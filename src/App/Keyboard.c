#include "Keyboard.h"
#include "Sys.h"

#define KEYBOARD_PREAMBLE_CODE1 0x57
#define KEYBOARD_PREAMBLE_CODE2 0xAB
#define KEYBOARD_USBCODE_LEN    8

typedef enum
{
	KB_KEY_LCTRL = 0,
	KB_KEY_RCTRL,
	KB_KEY_UP,
	KB_KEY_DOWN,
	KB_KEY_LEFT,
	KB_KEY_RIGHT,
	KB_KEY_ENTER1,
	KB_KEY_ENTER2,
	KB_KEY_COUNT,
}KeyboardKey_t;

static uint8_t g_keyBuff[64];
static uint8_t g_keyBuffCount = 0;
static volatile bool g_gotframe = false;
static KeyboardKeyHandle_t g_keyHandle = NULL;

static uint8_t g_usbcode[KB_KEY_COUNT][KEYBOARD_USBCODE_LEN] = {
	{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00},
};

static void keyEvent(KeyboardKey_t key)
{
    if(g_keyHandle)
    {
        g_keyHandle(key);
    }
}

static void parseKey(uint8_t *code)
{
	uint8_t i;
	bool gotkey = false;
    static SysTime_t lastCtrlTime;
    static bool ctrlPressed = false;

	for(i = 0; i < KB_KEY_COUNT; i++)
	{
		if(memcmp(g_usbcode[i], code, KEYBOARD_USBCODE_LEN) == 0)
		{
		    SysLog("got key");
			gotkey = true;
			break;
		}	
	}

	if(!gotkey)
	{
		return;
	}

	switch (i)
	{
	case KB_KEY_LCTRL:
    case KB_KEY_RCTRL:
        if(ctrlPressed && !SysTimeHasPast(lastCtrlTime, 1000))
        {
            keyEvent(KEYBOARD_KEY_ACTIVE);
            ctrlPressed = false;
        }
        else
        {
            ctrlPressed = true;
        }
        lastCtrlTime = SysTime();
		break;
	case KB_KEY_UP:
        keyEvent(KEYBOARD_KEY_UP);
		break;
	case KB_KEY_DOWN:
        keyEvent(KEYBOARD_KEY_DOWN);
		break;
	case KB_KEY_LEFT:
        keyEvent(KEYBOARD_KEY_LEFT);
		break;
	case KB_KEY_RIGHT:
        keyEvent(KEYBOARD_KEY_RIGHT);
		break;
	case KB_KEY_ENTER1:
	case KB_KEY_ENTER2:
        keyEvent(KEYBOARD_KEY_ENTER);
		break;
	default:
		break;
	}
}

void KeyboardDataRecv(uint8_t uart, uint8_t *data, uint16_t len)
{
	uint16_t i;
    static SysTime_t oldtime;

    if(g_gotframe)
    {
        return ;
    }

    if(SysTimeHasPast(oldtime, 100))
    {
        g_keyBuffCount = 0;
    }
    oldtime = SysTime();

	for(i = 0; i < len; i++)
	{
		g_keyBuff[g_keyBuffCount++] = data[i];
		if(g_keyBuffCount == 1)
		{
			if(KEYBOARD_PREAMBLE_CODE1 != data[i])
			{
				g_keyBuffCount = 0;
			}
		}
		else if(g_keyBuffCount == 2)
		{
			if(KEYBOARD_PREAMBLE_CODE2 != data[i])
			{
				g_keyBuffCount = 0;
			}
		}
		else if(g_keyBuffCount == 3)
		{
			if(0x01 != data[i]) //keyboard
			{
				g_keyBuffCount = 0;
			}
		}
		else if(g_keyBuffCount == 11)
		{
			//parseKey(&g_keyBuff[3]);
			g_gotframe = true;
			g_keyBuffCount = 0;
		}
	}
}

static void frameHandle(void)
{
    if(g_gotframe)
    {
        parseKey(&g_keyBuff[3]);
        g_gotframe = false;
    }
}

void KeyboardInit(KeyboardKeyHandle_t handle)
{
    HalUartConfig_t config;
    config.baudrate = 115200;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = KeyboardDataRecv;
    HalUartConfig(HAL_UART_PORT_KVM1KEY, &config);

    g_keyHandle = handle;
}

void KeyboardPoll(void)
{
    frameHandle();
}

