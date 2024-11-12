#include "Drivers/CRC.h"

void CRC_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
	CRC->CR |= CRC_CR_RESET;
}

void CRC_DeInit(void)
{
	RCC->AHB1ENR &= ~RCC_AHB1ENR_CRCEN;
}

uint32_t CRC_Calculate(const uint32_t *data, uint32_t len)
{
	CRC->CR |= CRC_CR_RESET;
	while(len--)
	{
		CRC->DR = *data++;
	}
	return CRC->DR;
}
