#include "Drivers/TIM.h"

// Global variables for timer state
volatile uint8_t countdown_timer = 8; // Initialize countdown to 8 seconds
volatile uint8_t timeout_flag = 0;    // Flag indicating timeout

// Initialize TIM2 for 1-second intervals
void TIM2_Init(void)
{
    // Enable TIM2 Clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Configure TIM2
    TIM2->PSC = 16000 - 1; // Prescaler: 16 MHz / 16000 = 1 kHz
    TIM2->ARR = 1000 - 1;  // Auto-reload: 1 kHz / 1000 = 1 Hz (1 second)

    TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt
}

// Start TIM2 Timer
void TIM2_Start(void)
{
    TIM2->CR1 |= TIM_CR1_CEN;   // Enable TIM2 counter
    NVIC_EnableIRQ(TIM2_IRQn);  // Enable TIM2 interrupt in NVIC
    NVIC_SetPriority(TIM2_IRQn, 1); // Set interrupt priority
}

// Stop TIM2 Timer
void TIM2_Stop(void)
{
    TIM2->CR1 &= ~TIM_CR1_CEN;  // Disable TIM2 counter
    NVIC_DisableIRQ(TIM2_IRQn); // Disable TIM2 interrupt in NVIC
}

// TIM2 Interrupt Handler
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    { // Check for update interrupt flag
        TIM2->SR &= ~TIM_SR_UIF; // Clear the flag

        if (countdown_timer > 0)
        {
            countdown_timer--; // Decrement countdown
        }
        else
        {
            timeout_flag = 1; // Set timeout flag when countdown reaches 0
            TIM2_Stop();      // Stop the timer
        }
    }
}
