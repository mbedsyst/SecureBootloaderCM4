#include "BOOT.h"

#include <stdio.h>
#include "Drivers/LED.h"
#include "Drivers/UART.h"
#include "Drivers/SYSTICK.h"
#include "Drivers/CRC.h"

void BOOT_Init(void)
{
	LED_Init();
	UART2_Init();
	CRC_Init();
	printf("[info] Initializing Bootloader.\n\r");
}

void BOOT_DeInit(void)
{
	printf("[info] De-Initializing Bootloader.\n\r");
	LED_DeInit();
	UART2_DeInit();
	CRC_DeInit();
}

void BOOT_LocateApplication()
{

}

void BOOT_VerifyApplication()
{

}

void BOOT_LoadApplication()
{

}

void BOOT_HandleErrors()
{

}
