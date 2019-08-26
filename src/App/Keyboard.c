#include "Keyboard.h"
//#include "Sys.h"
#include "VTStaticQueue.h"

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
	KB_KEY_ESC,
	KB_KEY_NUM1,
    KB_KEY_NUM1P,
    KB_KEY_NUM2,
    KB_KEY_NUM2P,
    KB_KEY_NUM3,
    KB_KEY_NUM3P,
    KB_KEY_NUM4,
    KB_KEY_NUM4P,
    KB_KEY_NUM5,
    KB_KEY_NUM5P,
    KB_KEY_NUM6,
    KB_KEY_NUM6P,
    KB_KEY_NUM7,
    KB_KEY_NUM7P,
    KB_KEY_NUM8,
    KB_KEY_NUM8P,
    KB_KEY_NUM9,
    KB_KEY_NUM9P,
    KB_KEY_NUM0,
    KB_KEY_NUM0P,
	KB_KEY_COUNT,
}KBKeyValue_t;

//static uint8_t g_keyBuff[64];
//static uint8_t g_keyBuffCount = 0;
//static volatile bool g_gotframe = false;
static KeyboardKeyHandle_t g_keyHandle = NULL;

static uint8_t g_usbcode[KB_KEY_COUNT][KEYBOARD_USBCODE_LEN] = {
	{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//lctrl
	{0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},//rctrl
	{0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00},//up
	{0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00},//down
	{0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00},//left
	{0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00},//right
	{0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00},//enter1
	{0x00, 0x00, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00},//enter2
    {0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00},//esc
    {0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00},//1
    {0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00},//1p
    {0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00},//2
    {0x00, 0x00, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00},//2p
    {0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00},//3
    {0x00, 0x00, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00},//1p
    {0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00},//4
    {0x00, 0x00, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x00},//4p
    {0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00},//5
    {0x00, 0x00, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x00},//5p
    {0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00},//6
    {0x00, 0x00, 0x5E, 0x00, 0x00, 0x00, 0x00, 0x00},//6p
    {0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00},//7
    {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00},//7p
    {0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00},//8
    {0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},//8p
    {0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00},//9
    {0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00},//9p
    {0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00},//0
    {0x00, 0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00},//0p
};

static void keyEvent(KeyboardID_t id, KeyboardKey_t key)
{
    if(g_keyHandle)
    {
        g_keyHandle(id, key);
    }
}

static void parseKey(Keyboard_t *kb, uint8_t *code)
{
	uint8_t i;
	bool gotkey = false;
    //static SysTime_t lastCtrlTime;
    //static bool ctrlPressed = false;

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

	switch ((KBKeyValue_t)i)
	{
    case KB_KEY_LCTRL:
    case KB_KEY_RCTRL:
        if(kb->ctrlPressed && (!SysTimeHasPast(kb->lastCtrlTime, 1000)))
        {
            keyEvent(kb->id, KEYBOARD_KEY_DOUBLE_CTRL);
            kb->ctrlPressed = false;
        }
        else
        {
            kb->ctrlPressed = true;
        }
        kb->lastCtrlTime = SysTime();
		break;
	case KB_KEY_UP:
        keyEvent(kb->id, KEYBOARD_KEY_UP);
		break;
	case KB_KEY_DOWN:
        keyEvent(kb->id, KEYBOARD_KEY_DOWN);
		break;
	case KB_KEY_LEFT:
        keyEvent(kb->id, KEYBOARD_KEY_LEFT);
		break;
	case KB_KEY_RIGHT:
        keyEvent(kb->id, KEYBOARD_KEY_RIGHT);
		break;
	case KB_KEY_ENTER1:
	case KB_KEY_ENTER2:
        keyEvent(kb->id, KEYBOARD_KEY_ENTER);
		break;
    case KB_KEY_ESC:
        keyEvent(kb->id, KEYBOARD_KEY_ESC);
        break;
    case KB_KEY_NUM1:
    case KB_KEY_NUM1P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM1);
        break;
    case KB_KEY_NUM2:
    case KB_KEY_NUM2P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM2);
        break;
    case KB_KEY_NUM3:
    case KB_KEY_NUM3P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM3);
        break;
    case KB_KEY_NUM4:
    case KB_KEY_NUM4P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM4);
        break;
    case KB_KEY_NUM5:
    case KB_KEY_NUM5P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM5);
        break;
    case KB_KEY_NUM6:
    case KB_KEY_NUM6P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM6);
        break;
    case KB_KEY_NUM7:
    case KB_KEY_NUM7P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM7);
        break;
    case KB_KEY_NUM8:
    case KB_KEY_NUM8P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM8);
        break;
    case KB_KEY_NUM9:
    case KB_KEY_NUM9P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM9);
        break;
    case KB_KEY_NUM0:
    case KB_KEY_NUM0P:
        keyEvent(kb->id, KEYBOARD_KEY_NUM0);
        break;
	default:
		break;
	}
}

void KeyboardRecvByte(Keyboard_t *kb, uint8_t data)
{
    if(kb == NULL)
    {
        return;
    }

    if(SysTimeHasPast(kb->lastTime, 100))
    {
        kb->buffcount = 0;
    }
    kb->lastTime = SysTime();

	kb->buff[kb->buffcount++] = data;
	if(kb->buffcount == 1)
	{
		if(KEYBOARD_PREAMBLE_CODE1 != data)
		{
			kb->buffcount = 0;
		}
	}
	else if(kb->buffcount == 2)
	{
		if(KEYBOARD_PREAMBLE_CODE2 != data)
		{
			kb->buffcount = 0;
		}
	}
	else if(kb->buffcount == 3)
	{
		if(0x01 != data) //keyboard
		{
			kb->buffcount = 0;
		}
	}
	else if(kb->buffcount == 11)
	{
		parseKey(kb, &kb->buff[3]);
		kb->buffcount = 0;
	}
}

static VTSQueueDef(uint8_t, g_kvm2Buff, 128);
static VTSQueueDef(uint8_t, g_kvm1Buff, 128);
static Keyboard_t g_kvm1, g_kvm2;

static void kvm1DataRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        if(VTSQueueHasSpace(g_kvm1Buff))
        {
            VTSQueuePush(g_kvm1Buff, data[i]);
        }
    }
}

static void kbKVM1Init(void)
{
    HalUartConfig_t config;
    config.baudrate = 115200;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = kvm1DataRecv;
    HalUartConfig(HAL_UART_PORT_KVM1KEY, &config);

    g_kvm1.buffcount= 0;
    g_kvm1.ctrlPressed = false;
    g_kvm1.id = KEYBOARD_ID_KVM1;
}

static void kvm2DataRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        if(VTSQueueHasSpace(g_kvm2Buff))
        {
            VTSQueuePush(g_kvm2Buff, data[i]);
        }
    }
}

static void kbKVM2Init(void)
{
    HalUartConfig_t config;
    config.baudrate = 115200;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = kvm2DataRecv;
    HalUartConfig(HAL_UART_PORT_KVM2KEY, &config);
    
    g_kvm2.buffcount= 0;
    g_kvm2.ctrlPressed = false;
    g_kvm2.id = KEYBOARD_ID_KVM2;
}


static void kbKVM2RecvHandle(void)
{
    uint8_t dbyte;
    while(VTSQueueCount(g_kvm2Buff))
    {
        //HalIRQEnable(false);
        dbyte = VTSQueueFront(g_kvm2Buff);
        //HalIRQEnable(true);
        VTSQueuePop(g_kvm2Buff);
        KeyboardRecvByte(&g_kvm2, dbyte);
    }
}

static void kbKVM1RecvHandle(void)
{
    uint8_t dbyte;
    while(VTSQueueCount(g_kvm1Buff))
    {
        //HalIRQEnable(false);
        dbyte = VTSQueueFront(g_kvm1Buff);
        //HalIRQEnable(true);
        VTSQueuePop(g_kvm1Buff);
        KeyboardRecvByte(&g_kvm1, dbyte);
    }
}

void KeyboardInit(KeyboardKeyHandle_t handle)
{
    kbKVM2Init();
    kbKVM1Init();
    g_keyHandle = handle;
}

void KeyboardPoll(void)
{
    kbKVM1RecvHandle();
    kbKVM2RecvHandle();
}

