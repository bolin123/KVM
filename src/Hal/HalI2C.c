#include "HalI2C.h"
#include "HalGPIO.h"
#include "HalWait.h"

#define HAL_I2C_SCL_PIN 0x1A
#define HAL_I2C_SDA_PIN 0x1B

//pb10
#define SCL_H         HalGPIOSetLevel(HAL_I2C_SCL_PIN, 1)
#define SCL_L         HalGPIOSetLevel(HAL_I2C_SCL_PIN, 0)

//pb11
#define SDA_H         HalGPIOSetLevel(HAL_I2C_SDA_PIN, 1)
#define SDA_L         HalGPIOSetLevel(HAL_I2C_SDA_PIN, 0)

#define SCL_read      HalGPIOGetLevel(HAL_I2C_SCL_PIN)
#define SDA_read      HalGPIOGetLevel(HAL_I2C_SDA_PIN)

static void I2C_delay(void)
{
    HalWaitUs(10);//100k
}

static void delay5ms(void)
{
    HalWaitMs(5);
}

static bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)
        return false;
	SDA_L;
	I2C_delay();
	if(SDA_read)
        return false;
	SDA_L;
	I2C_delay();
	return true;
}

static void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

static void I2C_Ack(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static void I2C_NoAck(void)
{
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static bool I2C_WaitAck(void)
{
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
        SCL_L;
        I2C_delay();
        return false;
	}
	SCL_L;
	I2C_delay();
	return true;
}

static void I2C_SendByte(uint8_t SendByte)
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
        if(SendByte&0x80)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }
        SendByte<<=1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

static unsigned char I2C_RadeByte(void)
{
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;
    while(i--)
    {
        ReceiveByte<<=1;
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if(SDA_read)
        {
            ReceiveByte|=0x01;
        }
    }
    SCL_L;
    return ReceiveByte;
}

bool HalI2CWriteByte(unsigned char slaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
  	if(!I2C_Start())
        return false;
    I2C_SendByte(slaveAddress);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(REG_Address);
    I2C_WaitAck();
    I2C_SendByte(REG_data);
    I2C_WaitAck();
    I2C_Stop();
    delay5ms();
    return true;
}

int HalI2CWriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data)
{
    uint8_t i;

    if(!I2C_Start())
        return false;
    I2C_SendByte(devAddr);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(regAddr);
    I2C_WaitAck();

    for(i = 0; i < length; i++)
    {
        I2C_SendByte(data[i]);
        I2C_WaitAck();
    }
    I2C_Stop();
    delay5ms();
    return 0;
}

unsigned char HalI2CReadByte(unsigned char slaveAddress, unsigned char REG_Address)
{
    unsigned char REG_data;

    if(!I2C_Start())
    {
        return false;
    }
    I2C_SendByte(slaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//éè?????eê?μ??·+?÷?tμ??·
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte((uint8_t)REG_Address);   //éè??μí?eê?μ??·
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(slaveAddress + 1);
    I2C_WaitAck();

    REG_data = I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return true;
    return REG_data;

}

int8_t HalI2CReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
    uint8_t i;

    if(!I2C_Start())
    {
        return false;
    }
    I2C_SendByte(devAddr); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//éè?????eê?μ??·+?÷?tμ??·
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(regAddr);   //éè??μí?eê?μ??·
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(devAddr + 1);
    I2C_WaitAck();

    for(i = 0; i < length - 1; i++)
    {
        data[i] = I2C_RadeByte();
        I2C_WaitAck();
    }
    data[length - 1] = I2C_RadeByte();
    I2C_NoAck();

    I2C_Stop();
    return length;
}


void HalI2CInit(void)
{
	HalGPIOConfig(HAL_I2C_SCL_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_I2C_SDA_PIN, HAL_IO_OUTPUT);
}

void HalI2CPoll(void)
{
}


