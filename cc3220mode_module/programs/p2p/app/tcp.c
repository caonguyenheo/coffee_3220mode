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
#include "userconfig.h"
#include "cc3220_system.h"
#include "uart_terminal.h"


int32_t  gMode = -1;
void * TcpThread(void *param)
{
	int32_t             RetVal ;

	char gversion_8char[9] = {0};
	get_version(gversion_8char);
	printBorder('*', 52);
	UART_PRINT("\n\r");
	UART_PRINT("\t\tCC3220 version= %s\r\n", gversion_8char);
	printBorder('*', 52);
	UART_PRINT("\n\r");

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
		ClearTerm();
		system_reboot();

	}
	else
	{
		UART_PRINT("REGISTER DONE\n\r");
		get_wireless_config();
		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
	}
	return 0;
}
