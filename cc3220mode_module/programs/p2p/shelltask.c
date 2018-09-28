/*
 * shelltask.c
 *
 *  Created on: Aug 27, 2018
 *      Author: Admin
 */
#include <stdlib.h>
#include <stdint.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>

/* Example/Board Header files */
#include "main_thread.h"
#include "uart_terminal.h"
appControlBlock     app_CB1;
int32_t cmd_set_wifi(char *param)
{
    char *ssid = NULL;
    char *passwd = NULL;
    int lRetVal;

    ssid = strtok(NULL, " ");
    if (ssid == NULL)
    {
        return -1;
    }
    passwd = strtok(NULL, " ");
    if (passwd == NULL)
    {
        return -1;
    }
    UART_PRINT("\r\n\t- TRY TO CONNECT: %s %s\r\n", ssid, passwd);
    Network_IF_DeInitDriver();
    lRetVal = Network_IF_InitDriver(ROLE_STA);
    if (lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    lRetVal = Network_IF_Connect2AP_static_ip(ssid, passwd, 5);
    return lRetVal;


}
int ParseNProcessCmd(char *pcCmdBuffer)
{
	char *pcInpString;
	int iRetVal = -1;
	char l_delim[2] = "-";
	pcInpString = strtok(pcCmdBuffer, l_delim);
	if (!strcmp(pcInpString, "at"))
	{
		pcInpString = strtok(NULL, " ");
		if (!strcmp(pcInpString, "wificonnect"))
		{
            iRetVal = cmd_set_wifi(pcInpString);
            if (iRetVal<0)
            {
                UART_PRINT("FAIL\r\n");
            }
            else
            {
                UART_PRINT("OK\r\n");
            }
            iRetVal = 0;
		}
		else if (!strcmp(pcInpString, "scanwifi"))
		{
			UART_PRINT("\r\nOK\r\n");
			iRetVal = Network_IF_provisionAP();
            if (iRetVal<0)
            {
                UART_PRINT("FAIL\r\n");
            }
            else
            {
                UART_PRINT("OK\r\n");
            }
            iRetVal = 0;
		}
		else if (!strcmp(pcInpString, "reboot"))
		{
			system_reboot();
            iRetVal = 0;
		}
	    else
	    {
	        UART_PRINT("\r\nUnsupported command\r\n");
	        return iRetVal;
	    }


	}


	return iRetVal;

}

void ShellTask(void *param)
{
    int lRetVal;
    char acCmdStore[512];
    while(!app_CB1.Exit)
    {
    	UART_PRINT("user:");
		lRetVal = GetCmd(acCmdStore, sizeof(acCmdStore));

		if (lRetVal < 0)
		{

			UART_PRINT("Command length exceeded 512 bytes \r\n");
		}
		else
		{
			lRetVal = ParseNProcessCmd(acCmdStore);
			if (lRetVal < 0)
			{
				UART_PRINT("Error in processing command\r\n");
			}
		}
    }
//    return 0;
}
