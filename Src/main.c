#include "main.h"

int main()
{
	BOOT_Init();

	BOOT_LocateApplication();

	if(BOOT_VerifyApplication())
	{
		BOOT_LoadApplication();
	}
	else
	{
		BOOT_HandleErrors();
	}

	BOOT_DeInit();

	while(1)
	{

	}
}
