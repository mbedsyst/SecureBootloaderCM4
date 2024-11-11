#include "BOOT.h"

void BOOT_Init(void)
{
	LED_Init();
	UART2_Init();
	CRC_Init();
	printf("[info] Initialized Bootloader peripherals.\n\r");
}
