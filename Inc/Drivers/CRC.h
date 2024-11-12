#ifndef __CRC_H__
#define __CRC_H__

#include "stm32f4xx.h"

void CRC_Init(void);
void CRC_DeInit(void);
uint32_t CRC_Calculate(const uint32_t *data, uint32_t len);

#endif
