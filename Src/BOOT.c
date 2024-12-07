#include "BOOT.h"

#include "Drivers/LED.h"
#include "Drivers/UART.h"
#include "Drivers/SYSTICK.h"
#include "Drivers/TIM.h"
#include "Drivers/CRC.h"
#include "Libraries/W25Qxx.h"

#define APP_MAGIC_NUMBER		0xDEADBEEF
#define APP_SLOT_START_ADDR  	0x08020000
#define APP_SLOT_COUNT			3
#define APP_SLOT_SIZE      		(128 * 1024)

#define UPDATE_BLOCK_SIZE 		256
#define EXTERNAL_APP_START_ADDR 0x000002C  // Example start address in external flash
#define EXTERNAL_APP_SIZE       128*1024

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
app_code_metadata_t UpdateMetadata;
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
	printf("\n\n\n\r");
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

static void ProcessFirmwareMetadata(uint8_t *firmware_metadata)
{
	uint32_t magic_number = *(uint32_t *)firmware_metadata;
	if (magic_number != APP_MAGIC_NUMBER)
	{
		printf("[error] Invalid Magic Number in External Flash.\n\r");
		return ;
	}
	memcpy(&UpdateMetadata, firmware_metadata, sizeof(app_code_metadata_t));
}

static int8_t VerifyFirmwareChecksum(uint32_t app_start_address)
{
    uint8_t buffer[UPDATE_BLOCK_SIZE];
    uint32_t crc_value;
    uint32_t expected_crc = UpdateMetadata.crc;
    uint32_t bytes_remaining = UpdateMetadata.size;
    uint32_t current_address = app_start_address;

    CRC_Init();

    while (bytes_remaining > 0)
    {
        uint16_t bytes_to_read = (bytes_remaining > UPDATE_BLOCK_SIZE) ? UPDATE_BLOCK_SIZE : bytes_remaining;
        W25Q_ReadData(current_address / 256, current_address % 256, buffer, bytes_to_read);
        crc_value = CRC_Calculate((const uint32_t *)buffer, (bytes_to_read + 3) / 4);
        bytes_remaining -= bytes_to_read;
        current_address += bytes_to_read;
    }

    if (crc_value == expected_crc)
    {

    	return 1;
    }
    else
    {

        return 0;
    }
}

static void CheckOldestVersion(app_code_metadata_t *codes, int num_slots)
{
	uint32_t oldest_version = codes[0].version;
	for(int i = 0; i < num_slots; i++)
	{
		if(codes[i].version < oldest_version)
		{
			oldest_version = codes[i].version;
		}
	}

	// ToDo Return oldest version data

}

static int FindDefaultApplication(app_code_metadata_t *codes, int num_slots)
{
    int default_index = -1;
    uint32_t latest_version = 0;

    for (int i = 0; i < num_slots; i++)
    {
        if (codes[i].valid && codes[i].version > latest_version)
        {
            latest_version = codes[i].version;
            default_index = i;
        }
    }
    return default_index;
}

// Function to choose application to boot
static int ChooseApplicationToBoot(app_code_metadata_t *codes, int num_slots)
{
    printf("Available Applications:\n");
    for (int i = 0; i < num_slots; i++)
    {
        if (codes[i].valid)
        {
            printf("[%d] %s (Version: %d, Size: %d bytes)\n\r", i + 1, codes[i].application_name, codes[i].version, codes[i].size);
        }
    }

    printf("Choose an application to boot (1-3):\n\r");
    printf("Booting the default application in 8 seconds...\n\r");

    TIM2_Init();  // Initialize the timer
    TIM2_Start(); // Start the timer
    char user_input[10];

    while (!timeout_flag)
    {
        if (fgets(user_input, sizeof(user_input), stdin) != NULL)
        {
            int choice = atoi(user_input);
            if (choice >= 1 && choice <= 3 && codes[choice - 1].valid)
            {
                TIM2_Stop(); // Stop the timer on valid input
                return choice - 1;
            }
            else
            {
                printf("Invalid choice. Please enter a valid option.\n\r");
            }
        }
    }
    printf("No selection made. Booting the default application...\n\r");
    return find_default_application(); // Return the default application
}

static void BootApplication(app_code_metadata_t *codes, int app_index)
{
    if (app_index < 0 || app_index > 2 || !codes[app_index].valid)
    {
        printf("Invalid application index or application not valid.\n\r");
        return;
    }
    uint32_t app_start_addr = codes[app_index].start_addr;
    uint32_t *app_vector_table = (uint32_t *)app_start_addr;

    BOOT_DeInit();

    __disable_irq();
    SCB->VTOR = app_start_addr;
    __set_MSP(app_vector_table[0]);
    void (*app_reset_handler)(void) = (void (*)(void))app_vector_table[1];
    app_reset_handler();

    while (1);
}

void FindApplications(void)
{
	FillSlotMetadata(SlotMetadata, APP_SLOT_COUNT);
	FillCodeMetadata(SlotMetadata, AppMetadata, APP_SLOT_COUNT);
	VerifyAppChecksum(AppMetadata, APP_SLOT_COUNT);
	PrintValidAppCount(AppMetadata, APP_SLOT_COUNT);
}

void CheckFirmwareUpdate(void)
{
	uint8_t firmware_metadata[44];
	int8_t err = 0;
	uint32_t oldest_app_address;
	W25Q_Init();
	W25Q_ReadData(0, 0, firmware_metadata, sizeof(firmware_metadata));
	err = ProcessFirmwareMetadata(firmware_metadata);
	if(err < 0)
	{
		printf("[error] Exiting Firmware Update routine.\n\r");
		break;
	}
	oldest_app_address = CheckOldestVersion(AppMetadata, APP_SLOT_COUNT);
}

void SelectApplicationToLoad(void)
{
	int app_index = 0;
	FindDefaultApplication(AppMetadata, APP_SLOT_COUNT);
	app_index = ChooseApplicationToBoot(AppMetadata, APP_SLOT_COUNT);
	BootApplication(AppMetadata, app_index);
}

void Bootloader_Run(void)
{
	BOOT_Init();
	FindApplications();
	CheckFirmwareUpdate();
	SelectApplicationToLoad();
}


