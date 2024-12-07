#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f4xx.h"

// Function prototypes
void FLASH_Unlock(void);
void FLASH_Lock(void);
int FLASH_EraseSector(uint8_t sector);
int FLASH_Write(uint32_t address, const uint8_t *data, uint32_t length);

#endif // FLASH_H



#endif
