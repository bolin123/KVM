#ifndef HAL_FLASH_H
#define HAL_FLASH_H

void HalFlashWrite(unsigned int addr, unsigned char *data, unsigned int len);
void HalFlashRead(unsigned int addr, unsigned char *buff, unsigned int len);

#endif // HAL_FLASH_H

