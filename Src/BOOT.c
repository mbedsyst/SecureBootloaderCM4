#include "BOOT.h"

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

static void BOOT_DeInit(void)
{
	printf("[info] De-Initializing Bootloader.\n\r");
	LED_DeInit();
	UART2_DeInit();
	CRC_DeInit();
}

uint32_t BOOT_LocateApplication(uint32_t app_id_address)
{
	uint32_t appID = *(volatile uint32_t *)app_id_address;
	if(appID != APP_ID)
	{
		printf("[error] Invalid Application ID: 0x%08X\n\r", (unsigned int)appID);
		return 0;
	}
	printf("[info] Valid Application ID: 0x%08X\n\r", (unsigned int)appID);
	uint32_t appSize = *(volatile uint32_t *)APP_SIZE_ADDR;
	printf("[info] Application Size: %u bytes\n\r", (unsigned int)appSize);

	return appSize;
}

bool BOOT_VerifyApplication(uint32_t app_size)
{
	uint32_t app_words = app_size/4;
	uint32_t appCRC = *(volatile uint32_t *)APP_CRC_ADDR;
	const uint32_t appCODE = *(const uint32_t *)APP_CODE_START;
	uint32_t calculated_CRC = CRC_Calculate((const uint32_t *)appCODE, app_words);

	if(calculated_CRC != appCRC)
	{
		printf("[error] Application Verification failed: Checksum error.\n\r");
		return false;
	}

	printf("[info] Application Verification success: Checksum pass.\n\r");
	return true;
}

void BOOT_LoadApplication()
{
	uint32_t app_code_start = APP_CODE_START;
	uint32_t *app_vector_table = (uint32_t *)app_code_start;
	uint32_t app_stack_pointer = app_vector_table[0];
	uint32_t app_reset_handler = app_vector_table[1];

	__disable_irq();

	BOOT_DeInit();

	SCB->VTOR = app_code_start;
	__set_MSP(app_stack_pointer);

	void (*reset_handler)(void) = (void (*)(void))app_reset_handler;
	reset_handler();
	while(1);
}

void BOOT_HandleErrors(void)
{
	printf("[error] Application Failed to Load.\n\r");
}
