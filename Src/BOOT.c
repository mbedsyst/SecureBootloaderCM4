#include "BOOT.h"

#include "Drivers/LED.h"
#include "Drivers/UART.h"
#include "Drivers/SYSTICK.h"
#include "Drivers/CRC.h"
#include "Libraries/W25Qxx.h"

#define APP_MAGIC_NUMBER		0xDEADBEEF
#define APP_SLOT_START_ADDR  	0x08020000
#define APP_SLOT_COUNT			3
#define APP_SLOT_SIZE      		(128 * 1024)

typedef struct
{
    uint32_t id;
    uint32_t size;
    uint32_t version;
    uint32_t crc;
    uint32_t timestamp;
    uint32_t valid;
    uint32_t start_addr;
    char application_name[16];
} app_code_metadata_t;

typedef struct
{
	uint32_t slot_address;
	uint8_t is_valid;
} app_slot_metadata_t;


static void PrintBanner(void)
{

	printf(" _  ______  ______  ______  ______  ______  _____  ______  ______  ______  ______  ______  _ \n\r");
	printf("|_||______||______||______||______||______||_____||______||______||______||______||______||_|\n\r");
	printf("|_|                                                                                       |_|\n\r");
	printf("|_|  ███████ ███████  ██████ ██    ██ ██████  ███████ ██████   ██████   ██████  ████████  |_|\n\r");
	printf("|_|  ██      ██      ██      ██    ██ ██   ██ ██      ██   ██ ██    ██ ██    ██    ██     |_|\n\r");
	printf("|_|  ███████ █████   ██      ██    ██ ██████  █████   ██████  ██    ██ ██    ██    ██     |_|\n\r");
	printf("|_|       ██ ██      ██      ██    ██ ██   ██ ██      ██   ██ ██    ██ ██    ██    ██     |_|\n\r");
	printf("|_|  ███████ ███████  ██████  ██████  ██   ██ ███████ ██████   ██████   ██████     ██     |_|\n\r");
	printf("|_| ______  ______  ______  ______  ______  _____  ______  ______  ______  ______  ______ |_|\n\r");
	printf("|_||______||______||______||______||______||_____||______||______||______||______||______||_|\n\r");
	printf("\n\r");
	printf("\n\r");
	printf("\n\r");
	printf("[info] Initializing LED\n\r");
	printf("[info] Initializing UART\n\r");
	printf("[info] Initializing CRC\n\r");
	printf("[info] Initializing SPI\n\r");
	printf("[info] Initializing External Flash Memory\n\r");
	printf("[info] Bootloader Initialized...\n\r");

}

void BOOT_Init(void)
{
	LED_Init();
	UART2_Init();
	CRC_Init();
	W25Q_Init();
	PrintBanner();

}

static void BOOT_DeInit(void)
{
	printf("[info] De-Initializing Bootloader.\n\r");
	printf("[info] Loading Application Code.\n\r");
	LED_DeInit();
	UART2_DeInit();
	CRC_DeInit();
}

static void FillSlotMetadata(app_slot_metadata_t *slots, int num_slots)
{
	for(int i = 0; i < num_slots; i++)
	{
		slots[i].slot_address = (APP_SLOT_START_ADDR + i * APP_SLOT_SIZE);
		slots[i].is_valid = 0;

		uint32_t *slotStart = (uint32_t *)slots[i].slot_address;

		if(*slotStart == APP_MAGIC_NUMBER)
		{
			slots[i].is_valid = 1;
		}
	}
}

static void FillCodeMetadata(app_slot_metadata_t *slots, app_code_metadata_t *codes, int num_slots)
{
	for(int i = 0; i < num_slots; i++)
	{
		if(slots[i].is_valid)
		{
			codes[i].id 		= *(volatile uint32_t *)(APP_SLOT_START_ADDR + (i * APP_SLOT_SIZE) + 0);
			codes[i].size 		= *(volatile uint32_t *)(APP_SLOT_START_ADDR + (i * APP_SLOT_SIZE) + 1);
			codes[i].version 	= *(volatile uint32_t *)(APP_SLOT_START_ADDR + (i * APP_SLOT_SIZE) + 2);
			codes[i].crc 		= *(volatile uint32_t *)(APP_SLOT_START_ADDR + (i * APP_SLOT_SIZE) + 3);
			codes[i].timestamp	= *(volatile uint32_t *)(APP_SLOT_START_ADDR + (i * APP_SLOT_SIZE) + 4);
			codes[i].start_addr = (uint32_t)(APP_SLOT_START_ADDR + (i * APP_SLOT_SIZE) + 10);
		}
	}
}

static void VerifyAppChecksum(app_code_metadata_t *codes, int num_slots)
{
	for(int i = 0; i < num_slots; i++)
	{
		const uint32_t app_code_start= *(const uint32_t *)(codes[i].start_addr);
		uint32_t app_crc = CRC_Calculate((const uint32_t *)app_code_start, codes[i].size/4);
		if(app_crc != codes[i].crc)
		{
			codes[i].valid = 0;
		}
		else
		{
			codes[i].valid = 1;
		}
	}
}

static void ShowAvailableApplications(app_code_metadata_t *codes, int num_slots)
{
	printf("S.No.\tApplication\tSize\tVersion\tTimestamp\n\r");
	for(int i = 0; i < num_slots; i++)
	{
		printf("%d\t%s\t%d\t%d\t%d\n\r", i+1, codes[i].application_name, (int)codes[i].size, (int)codes[i].version, (int)codes[i].timestamp);
	}
	printf("\n\nChoose the Application to Boot, else latest version will be loaded.\n\r");
}

static void ChooseApplication(app_code_metadata_t *codes, int num_slots)
{
	uint8_t option;
	printf("[info] Available Applications in Storage:\n\r");
	ShowAvailableApplications(codes, APP_SLOT_COUNT);
	scanf("%d", &option);
	return option;

}

static void FindApplications(void)
{
	static app_code_metadata_t applicationCodeMetadata[3];
	static app_slot_metadata_t applicationSlotMetadata[3];

	FillSlotMetadata(applicationSlotMetadata, APP_SLOT_COUNT);
	FillCodeMetadata(applicationSlotMetadata, applicationCodeMetadata, APP_SLOT_COUNT);
	VerifyAppChecksum(applicationCodeMetadata, APP_SLOT_COUNT);
	ChooseApplication(applicationCodeMetadata, APP_SLOT_COUNT);
}


uint32_t BOOT_LocateApplication(uint32_t app_id_address)
{

	FindApplications();
/*	uint32_t appID = *(volatile uint32_t *)app_id_address;
	if(appID != APP_ID)
	{
		printf("\033[0;31m[error] Application missing at: 0x%08X\033[0m\n\r", (unsigned int)app_id_address);
		return 0;
	}
	printf("[info] Application found at: 0x%08X\n\r", (unsigned int)app_id_address);
	BOOT_SaveMetadata();
	return applicationMetadata.size;*/
}


static void BOOT_FirmwareUpdate(void)
{

}

bool BOOT_VerifyApplication(uint32_t app_size)
{
/*	uint32_t app_words = applicationMetadata.size/4;
	uint32_t appCRC = applicationMetadata.crc;
	const uint32_t appCODE = *(const uint32_t *)APP_CODE_START;
	uint32_t calculated_CRC = CRC_Calculate((const uint32_t *)appCODE, app_words);

	if(calculated_CRC != appCRC)
	{
		printf("\033[0;31m[error] Application Verification failed: Checksum error.\033[0m\n\r");
		return false;
	}

	printf("[info] Application Verification success: Checksum pass.\n\r");
	return true;*/
}

void BOOT_LoadApplication()
{/*
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
	while(1);*/
}

void BOOT_HandleErrors(void)
{
	printf("\033[0;31m[error] Application Failed to Load.\033[0m\n\r");
}
