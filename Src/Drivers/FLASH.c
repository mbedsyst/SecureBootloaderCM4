#include "Drivers/FLASH.h"

#define FLASH_SECTOR_BASE_ADDRESS  0x08000000U


void FLASH_Unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}


void FLASH_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}


int FLASH_EraseSector(uint8_t sector)
{

    if (sector > 11)
    {
        return -1;
    }

    while (FLASH->SR & FLASH_SR_BSY);

    FLASH_Unlock();

    FLASH->CR &= ~FLASH_CR_SNB;
    FLASH->CR |= (sector << FLASH_CR_SNB_Pos);
    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR |= FLASH_CR_STRT;
    while (FLASH->SR & FLASH_SR_BSY);
    if (FLASH->SR & (FLASH_SR_PGSERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR))
    {
        FLASH->SR |= (FLASH_SR_PGSERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR);
        FLASH_Lock();
        return -1;
    }
    FLASH->CR &= ~FLASH_CR_SER;
    FLASH_Lock();

    return 0;
}

int FLASH_Write(uint32_t address, const uint8_t *data, uint32_t length)
{
    if (address % 4 != 0)
    {
        return -1;
    }
    while (FLASH->SR & FLASH_SR_BSY);

    FLASH_Unlock();

    FLASH->CR |= FLASH_CR_PG;

    for (uint32_t i = 0; i < length; i += 4)
    {
        uint32_t word = *(uint32_t *)&data[i];
        *(volatile uint32_t *)address = word;
        while (FLASH->SR & FLASH_SR_BSY);
        if (FLASH->SR & (FLASH_SR_PGSERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR))
        {
            FLASH->SR |= (FLASH_SR_PGSERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR);
            FLASH_Lock();
            return -1;
        }
        address += 4;
    }
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH_Lock();

    return 0;
}
