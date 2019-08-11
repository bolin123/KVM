#ifndef HALI2C_H
#define HALI2C_H

#include "HalCtype.h"

bool HalI2CWriteByte(unsigned char slaveAddress,unsigned char REG_Address,unsigned char REG_data);
int HalI2CWriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);
unsigned char HalI2CReadByte(unsigned char slaveAddress, unsigned char REG_Address);
int8_t HalI2CReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

void HalI2CInit(void);
void HalI2CPoll(void);
#endif

