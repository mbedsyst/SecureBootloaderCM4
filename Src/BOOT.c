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

app_code_metadata_t AppMetadata[3];
app_slot_metadata_t SlotMetadata[3];

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

static void PrintValidAppCount(app_code_metadata_t *codes, int num_slots)
{
	uint8_t count = 0;
	for(int i = 0; i < num_slots; i++)
	{
		if(codes[i].valid == 1)
		{
			count++;
		}
	}
	printf("[info] Found %d valid applications in the Internal Flash Memory\n\r", count);
}

void FindApplications(void)
{
	FillSlotMetadata(SlotMetadata, APP_SLOT_COUNT);
	FillCodeMetadata(SlotMetadata, AppMetadata, APP_SLOT_COUNT);
	VerifyAppChecksum(AppMetadata, APP_SLOT_COUNT);
	PrintValidAppCount(AppMetadata, APP_SLOT_COUNT);
}

