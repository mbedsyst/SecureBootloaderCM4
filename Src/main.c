#include "main.h"

int main()
{
	uint32_t applicationSize;
	BOOT_Init();

	applicationSize = BOOT_LocateApplication(APP_ID_ADDR);

	if(BOOT_VerifyApplication(applicationSize))
	{
		BOOT_LoadApplication();
	}
	else
	{
		BOOT_HandleErrors();
	}

	while(1)
	{

	}
}
