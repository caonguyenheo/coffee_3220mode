/*
 * shelltask.c
 *
 *  Created on: Aug 27, 2018
 *      Author: Admin
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>

/* Example/Board Header files */
#include "main_thread.h"
#include "tcp.h"
#include "shelltask.h"
#include "system.h"
#include "network_handler.h"
#include "sflash.h"

int ParseNProcessCmd(char *pcCmdBuffer);
int32_t cmd_set_wifi(char *param);
int32_t cmd_set_tcp(char *param);
int32_t cmd_get_tcp(char *param);
int32_t cmd_get_tcp_file (char *param);
int32_t cmd_set_tcp_file (char *param);

void * ShellThread(void *param)
{
    int lRetVal;
    char acCmdStore[512];
    while(FOREVER)
    {
    	UART_PRINT("\n\r");
    	UART_PRINT("user:");
    	UART_PRINT("\n\r");
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
				UART_PRINT("Error in processing command\n\r");
			}
		}
    }

//    return ;
}

int ParseNProcessCmd(char *pcCmdBuffer)
{
	char *pcInpString;
	int iRetVal = -1;
	pcInpString = strtok(pcCmdBuffer, " ");

	if (!strcmp(pcInpString, "help"))
	{
		UART_PRINT("\n\r");
		DisplayUsage();
		iRetVal = 0;
	}
	else if (!strcmp(pcInpString, "connect"))
	{
		iRetVal = cmd_set_wifi(pcInpString);
		if (iRetVal<0)
		{
			UART_PRINT("FAIL\n\r");
		}
		else
		{
			UART_PRINT("OK\n\r");
			GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
		}
		iRetVal = 0;
	}

	else if (!strcmp(pcInpString, "reboot"))
	{
		UART_PRINT("\n\r");
		system_reboot();
		iRetVal = 0;
	}
	else if (!strcmp(pcInpString, "disconnect"))
	{
		UART_PRINT("\n\r");
		sl_WlanProfileDel(0xFF);
		sl_WlanPolicySet(SL_WLAN_POLICY_SCAN, SL_SCAN_POLICY(0), 0, 0);
		iRetVal = Network_IF_DeInitDriver();
		if (iRetVal<0)
		{
			UART_PRINT("FAIL\n\r");
		}
		else
		{
			UART_PRINT("OK\n\r");
			GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_ON);
		}
		system_reboot();
		iRetVal = 0;
	}
	else if (!strcmp(pcInpString, "recv"))
	{

		iRetVal = cmd_get_tcp(pcInpString);
		if(iRetVal < 0)
		{
			UART_PRINT("FAIL\n\r");
		}
		else
		{
			UART_PRINT("OK\n\r");
		}
		iRetVal = 0;
	}

	else if (!strcmp(pcInpString, "send"))
	{
		iRetVal = cmd_set_tcp(pcInpString);
		if(iRetVal < 0)
		{
			UART_PRINT("FAIL\n\r");
		}
		else
		{
			UART_PRINT("OK\n\r");
		}
		iRetVal = 0;
	}
	else if (!strcmp(pcInpString, "sendfile"))
	{
		iRetVal = cmd_set_tcp_file(pcInpString);
		if(iRetVal < 0)
		{
			UART_PRINT("FAIL\n\r");
		}
		else
		{
			UART_PRINT("OK\n\r");
		}
		iRetVal = 0;
	}
	else if (!strcmp(pcInpString, "recvfile"))
	{
		iRetVal = cmd_get_tcp_file(pcInpString);
		if(iRetVal < 0)
		{
			UART_PRINT("FAIL\n\r");
		}
		else
		{
			UART_PRINT("OK\n\r");
		}
		iRetVal = 0;
	}
	else
	{
		UART_PRINT("Unsupported command\n\r");
		return iRetVal;
	}
	return iRetVal;
}

/*
int ParseNProcessCmd(char *pcCmdBuffer)
{
	char *pcInpString;
	int iRetVal = -1;
	char l_delim[2] = "-";
	pcInpString = strtok(pcCmdBuffer, l_delim);
	if (!strcmp(pcInpString, "at"))
	{
		pcInpString = strtok(NULL, " ");
		if (!strcmp(pcInpString, "connect"))
		{
            iRetVal = cmd_set_wifi(pcInpString);
            if (iRetVal<0)
            {
                UART_PRINT("FAIL\r\n");
            }
            else
            {
                UART_PRINT("OK\r\n");
                GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
            }
            iRetVal = 0;
		}

		else if (!strcmp(pcInpString, "reboot"))
		{
			system_reboot();
            iRetVal = 0;
		}
		else if (!strcmp(pcInpString, "disconnect"))
		{
		    iRetVal = Network_IF_DisconnectFromAP();
		    sl_Stop(SL_STOP_TIMEOUT);
		    Network_IF_ResetMCUStateMachine();
		    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_ON);
		    system_reboot();
		    iRetVal = 0;
		}

		else if (!strcmp(pcInpString, "recvp"))
		{
		    iRetVal = 0;
		}

		else if (!strcmp(pcInpString, "senp"))
		{
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
*/

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
int32_t cmd_set_tcp(char *param)
{
    char *ip_str = NULL;
    char *port = NULL;
    char *p_optionStr = NULL;
    char *ip_optionStr = NULL;
    char *data = NULL;
    char *nb = NULL;
    char *tx = NULL;
    unsigned short usPort = 0;
    int inb = 0;
    int itx = 0;
    int iRetVal;
    uint32_t ip_temp = 0;

    ip_optionStr  = strtok(NULL, " ");
    if(!strcmp(ip_optionStr, "-i"))
    {
        UART_PRINT("\n\r");
    	UART_PRINT("Prepare Send Data To Ip\t");
    }
    else
    {
    	return -1;
    }

    ip_str = strtok(NULL, " ");
	if (ip_str == NULL)
	{
		return -1;
	}
	else
	{
		UART_PRINT("%s ", ip_str);
	}

    p_optionStr  = strtok(NULL, " ");
    if(!strcmp(p_optionStr, "-p"))
    {
    	UART_PRINT("And Port Number ");
    }
    else
    {
    	return -1;
    }

	port = strtok(NULL, " ");
	if (port == NULL)
	{
		return -1;
	}
	else
	{
	    usPort = atoi(port);
	    UART_PRINT("%d\n\r", usPort);
	}

	data = strtok(NULL, " ");
	if (data == NULL)
	{
		return -1;
	}
	else
	{
	    UART_PRINT("%s\n\r", data);
	}

	nb = strtok(NULL, " ");
	if (nb == NULL)
	{
		return -1;
	}
	else
	{
		inb = atoi(nb);
	    UART_PRINT("inb:%d\n\r", inb);
	}

	tx = strtok(NULL, " ");
	if (tx == NULL)
	{
		return -1;
	}
	else
	{
		itx = atoi(tx);
	    UART_PRINT("tx:%d\n\r", itx);
	}


	ipv4AddressParse(ip_str, &ip_temp);
	UART_PRINT("ip_temp:%x\n\r", ip_temp);
    iRetVal = TcpClient(ip_temp, usPort, data, inb, itx);

    return iRetVal;
}
int32_t cmd_get_tcp(char *param)
{
    char *port = NULL;
    char *nb = NULL;
    unsigned short usPort = 0;
    int inb = -1;
    char *p_optionStr = NULL;

    char *tx = NULL;
    int itx = 0;

    int iRetVal;

    p_optionStr  = strtok(NULL, " ");
    if(!strcmp(p_optionStr, "-p"))
    {
        UART_PRINT("\n\r");
    	UART_PRINT("Prepare Received Data and Packet On Port Number: ");
    }
    else
    {
    	return -1;
    }

    port = strtok(NULL, " ");
    if (port == NULL)
    {
        return -1;
    }
    else
    {
        usPort = atoi(port);
        UART_PRINT("%d\n\r", usPort);
    }
    nb = strtok(NULL, " ");
    if (nb == NULL)
    {
        return -1;
    }
	else
	{
		inb = atoi(nb);
	    UART_PRINT("inb:%d\n\r", inb);
	}

	tx = strtok(NULL, " ");
	if (tx == NULL)
	{
		return -1;
	}
	else
	{
		itx = atoi(tx);
	    UART_PRINT("tx:%d\n\r", itx);
	}
    iRetVal = TcpServer(usPort, inb, itx);

	return iRetVal;
}
int32_t cmd_set_tcp_file (char *param)
{
	char *ip_str = NULL;
	char *port = NULL;
	char *p_optionStr = NULL;
	char *ip_optionStr = NULL;
//	int iRetVal;

    unsigned short usPort = 0;
    uint32_t ip_temp = 0;

    ip_optionStr  = strtok(NULL, " ");
    if(!strcmp(ip_optionStr, "-i"))
    {
        UART_PRINT("\n\r");
    	UART_PRINT("Prepare Send Data To Ip\t");
    }
    else
    {
    	return -1;
    }

    ip_str = strtok(NULL, " ");
	if (ip_str == NULL)
	{
		return -1;
	}
	else
	{
		UART_PRINT("%s ", ip_str);
	}

    p_optionStr  = strtok(NULL, " ");
    if(!strcmp(p_optionStr, "-p"))
    {
    	UART_PRINT("And Port Number ");
    }
    else
    {
    	return -1;
    }

	port = strtok(NULL, " ");
	if (port == NULL)
	{
		return -1;
	}
	else
	{
	    usPort = atoi(port);
	    UART_PRINT("%d\n\r", usPort);
	}
	ipv4AddressParse(ip_str, &ip_temp);
	UART_PRINT("ip_temp:%x\n\r", ip_temp);
	SendFile_Tcp(ip_temp, usPort);
    return 0;
}
int32_t cmd_get_tcp_file (char *param)
{

    char *port = NULL;
    unsigned short usPort = 0;
    char *p_optionStr = NULL;

    int iRetVal;

    p_optionStr  = strtok(NULL, " ");
    if(!strcmp(p_optionStr, "-p"))
    {
        UART_PRINT("\n\r");
    	UART_PRINT("Prepare Received Data and Packet On Port Number: ");
    }
    else
    {
    	return -1;
    }

    port = strtok(NULL, " ");
    if (port == NULL)
    {
        return -1;
    }
    else
    {
        usPort = atoi(port);
        UART_PRINT("%d\n\r", usPort);
    }

    iRetVal = ReceFile_Tcp(usPort);

	return iRetVal;
}
