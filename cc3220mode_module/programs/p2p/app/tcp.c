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

void * TcpThread(void *param)
{
	int32_t             RetVal ;
	sl_Start(0, 0, 0);
	system_init();
	RetVal = system_IsRegistered();
	UART_PRINT("system_IsRegistered : %d\n\r",RetVal);
	if((RetVal < 0))
	{
		system_registration();
	}
	else
	{
		UART_PRINT("REGISTER DONE\n\r");
	}
	return 0;
}
