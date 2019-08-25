#include "HalFlash.h"
#include "HalCtype.h"
#include "stm32f4xx_flash.h"

static uint32_t getSector(uint32_t addr)
{
    return FLASH_Sector_4;
}

void HalFlashWrite(uint32_t addr, uint8_t *data, uint32_t len)
{
    int i, j;
    uint32_t g_sun = 0x00000000;
    uint32_t Address = addr;
    uint8_t num = 0;
    uint8_t *str = data;

    
    FLASH_Unlock();  
    FLASH_DataCacheCmd(DISABLE);
    
    for(i = 0; i < 10; i++)
    {
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR);
        if(FLASH_EraseSector(getSector(addr), VoltageRange_3) == FLASH_COMPLETE)
        {
            break;
        }
    }

    if(data != 0)
	{
		for(j = len / 4; j >= 0; j--)
		{
			g_sun = 0;
			if(j == 0)
			{
				num = len % 4;
				switch(num)
				{
					case 1:
					case 2:
					case 3:		
					for(i = num - 1 ; i >= 0; i--)
					{
						g_sun |= str[i];
						if(i > 1)
						{
							g_sun = (g_sun << 8);
						}
						else
						{
							g_sun = (g_sun << 8*i);
						}
					}		
					break;
				}
			}
			else 
			{
					for(i = 3; i >= 1; i--)
					{
						g_sun |= str[i];
						g_sun = (g_sun << 8); 
					}		
					g_sun |= str[0];
			}
			str =str + 4;
			if (FLASH_ProgramWord(Address, g_sun) == FLASH_COMPLETE)
			{
				Address = Address + 4;
			}
			else
			{ 
			}
		}
	}
    
    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();
}


void HalFlashRead(uint32_t addr, uint8_t *buf, uint32_t len)
{
	memcpy(buf, (const void *)addr, len);
}

void FLASH_IRQHandler(void)
{
	printf("%s Entry!!!\n", __func__);
	//while(1);
}



