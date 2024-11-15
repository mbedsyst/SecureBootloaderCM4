#ifndef BOOT_H_
#define BOOT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define APP_HEADER_ADDR		0x08008000
#define APP_ID_ADDR			APP_HEADER_ADDR
#define APP_ID				0xFACADE01
#define APP_VERSION_ADDR	APP_HEADER_ADDR + 4
#define APP_SIZE_ADDR		APP_HEADER_ADDR + 8
#define APP_CRC_ADDR		APP_HEADER_ADDR + 12
#define APP_CODE_START		APP_HEADER_ADDR + 16

typedef struct
{
	uint32_t id;
	uint32_t version;
	uint32_t size;
	uint32_t crc;
} app_Metadata;

void BOOT_Init(void);
uint32_t BOOT_LocateApplication(uint32_t app_id_address);
bool BOOT_VerifyApplication(uint32_t app_size);
void BOOT_LoadApplication();
void BOOT_HandleErrors();

#endif
