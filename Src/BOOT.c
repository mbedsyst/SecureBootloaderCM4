#include "BOOT.h"

#include "Drivers/LED.h"
#include "Drivers/UART.h"
#include "Drivers/SYSTICK.h"
#include "Drivers/CRC.h"

static app_Metadata applicationMetadata;

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

static BOOT_SaveMetadata(void)
{
	applicationMetadata.id = *(volatile uint32_t *)APP_ID_ADDR;
	applicationMetadata.version = *(volatile uint32_t *)APP_VERSION_ADDR;
	applicationMetadata.size = *(volatile uint32_t *)APP_SIZE_ADDR;
	applicationMetadata.crc = *(volatile uint32_t *)APP_CRC_ADDR;

	printf("[info] Application ID: 0x%08X\r\n", applicationMetadata.id);
	printf("[info] Application Version: %u\r\n", applicationMetadata.version);
	printf("[info] Application File size: %u bytes\r\n", applicationMetadata.size);
	printf("[info] Application CRC Value: 0x%08X\r\n",applicationMetadata.crc);
}

uint32_t BOOT_LocateApplication(uint32_t app_id_address)
{
	uint32_t appID = *(volatile uint32_t *)app_id_address;
	if(appID != APP_ID)
	{
		printf("[error] Application missing at: 0x%08X\n\r", (unsigned int)appID);
		return 0;
	}
	printf("[info] Application found at: 0x%08X\n\r", (unsigned int)app_id_address);
	BOOT_SaveMetadata();
	return applicationMetadata.size;
}

bool BOOT_VerifyApplication(uint32_t app_size)
{
	uint32_t app_words = applicationMetadata.size/4;
	uint32_t appCRC = applicationMetadata.crc;
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
