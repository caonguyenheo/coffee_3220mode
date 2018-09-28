/*
 * tcp.c
 *
 *  Created on: Aug 30, 2018
 *      Author: Admin
 */

#include "tcp.h"
#include "shelltask.h"
#include "system.h"
#include "network_handler.h"
#include "sflash.h"
#include "main_thread.h"


int32_t  gMode = -1;
void * TcpThread(void *param)
{
	int32_t             RetVal ;

	gMode = sl_Start(0, 0, 0);
	UART_PRINT("Mode sl_Start: %d\n\r",gMode);
	if(gMode > 0)
	{
		system_init();
	}
	else
	{
		userconfig_flash_read();
	}

	RetVal = system_IsRegistered();
	UART_PRINT("system_IsRegistered : %d\n\r",RetVal);
	if((RetVal < 0))
	{

		system_registration();
		osi_Sleep(1000);
		system_reboot();
	}
	else
	{
		UART_PRINT("REGISTER DONE\n\r");
		get_wireless_config();
	}
	return 0;
}
