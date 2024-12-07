#ifndef __TIM_H__
#define __TIM_H__

#include "stm32f4xx.h"

// Function Prototypes
void TIM2_Init(void);
void TIM2_Start(void);
void TIM2_Stop(void);

// Externally accessible variables
extern volatile uint8_t countdown_timer;
extern volatile uint8_t timeout_flag;

#endif
