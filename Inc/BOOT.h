#ifndef BOOT_H_
#define BOOT_H_

#define APP_START_ADDR	0x08008000
#define MAGIC_NUM_ADDR	APP_START_ADDR
#define APP_SIZE_ADDR	APP_START_ADDR + 4
#define APP_CODE_START	APP_START_ADDR + 8

void BOOT_Init(void);
void BOOT_DeInit(void);
void BOOT_LocateApplication();
void BOOT_VerifyApplication();
void BOOT_LoadApplication();
void BOOT_HandleErrors();

#endif
