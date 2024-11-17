#ifndef BOOT_H_
#define BOOT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


void BOOT_Init(void);
uint32_t BOOT_LocateApplication(uint32_t app_id_address);
bool BOOT_VerifyApplication(uint32_t app_size);
void BOOT_LoadApplication();
void BOOT_HandleErrors();

#endif
