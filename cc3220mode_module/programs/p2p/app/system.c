/*
 * system.c
 *
 *  Created on: Aug 30, 2018
 *      Author: Admin
 */

#include "system.h"
#include "tcp.h"
#include "shelltask.h"
#include "network_handler.h"

char lineBreak[]                = "\n\r";
unsigned long  g_ulDestinationIp = IP_ADDR;
unsigned int   g_uiPortNum = PORT_NUM;
volatile unsigned long  g_ulPacketCount = TCP_PACKET_COUNT;
unsigned char  g_ucConnectionStatus = 0;
unsigned char  g_ucSimplelinkstarted = 0;
unsigned long  g_ulIpAddr = 0;
char g_cBsdBuf[BUFF_SIZE];
void printBorder(char ch, int n)
{
    int        i = 0;

    for(i=0; i<n; i++)    putch(ch);
}
void DisplayUsage()
{
	UART_PRINT("\t=========================================================================================\r\n");
	UART_PRINT("\t\t\t\tAT COMMAND SUPPORT:\r\n");
    UART_PRINT("\tconnect [SSID] [PASSWD]\t");
    UART_PRINT("\tdisconnect\t");
    UART_PRINT("\treboot\n\r");
    UART_PRINT("\tsend -i [ip] -p [port number]\t");
    UART_PRINT("recv -p [port number]\n\r");

    UART_PRINT("\t=========================================================================================\r\n");
}
int32_t DisplayAppBanner(char* appName, char* appVersion)
{
     int32_t            ret = 0;
     uint8_t            macAddress[SL_MAC_ADDR_LEN];
     uint16_t           macAddressLen = SL_MAC_ADDR_LEN;
     uint16_t           ConfigSize = 0;
     uint8_t            ConfigOpt = SL_DEVICE_GENERAL_VERSION;
     SlDeviceVersion_t ver = {0};

     ConfigSize = sizeof(SlDeviceVersion_t);

    /* Print device version info. */
    ret =
        sl_DeviceGet(SL_DEVICE_GENERAL, &ConfigOpt, &ConfigSize,
                     (uint8_t*)(&ver));
    ASSERT_ON_ERROR(ret, DEVICE_ERROR);

    /* Print device Mac address */
    ret = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
                       &macAddress[0]);
    ASSERT_ON_ERROR(ret, WLAN_ERROR);

    UART_PRINT(lineBreak);
    UART_PRINT("\t");
    printBorder('=', 46);
    UART_PRINT(lineBreak);
    UART_PRINT("\t   %s Ver: %s",appName, appVersion);
    UART_PRINT(lineBreak);
    UART_PRINT("\t");
    printBorder('=', 46);
    UART_PRINT(lineBreak);
    UART_PRINT(lineBreak);
    UART_PRINT("\t CHIP: 0x%x",ver.ChipId);
    UART_PRINT(lineBreak);
    UART_PRINT("\t MAC:  %d.%d.%d.%d",ver.FwVersion[0],ver.FwVersion[1],
               ver.FwVersion[2],
               ver.FwVersion[3]);
    UART_PRINT(lineBreak);
    UART_PRINT("\t PHY:  %d.%d.%d.%d",ver.PhyVersion[0],ver.PhyVersion[1],
               ver.PhyVersion[2],
               ver.PhyVersion[3]);
    UART_PRINT(lineBreak);
    UART_PRINT("\t NWP:  %d.%d.%d.%d",ver.NwpVersion[0],ver.NwpVersion[1],
               ver.NwpVersion[2],
               ver.NwpVersion[3]);
    UART_PRINT(lineBreak);
    UART_PRINT("\t ROM:  %d",ver.RomVersion);
    UART_PRINT(lineBreak);
    UART_PRINT("\t HOST: %s", SL_DRIVER_VERSION);
    UART_PRINT(lineBreak);
    UART_PRINT("\t MAC address: %02x:%02x:%02x:%02x:%02x:%02x",
               macAddress[0],
               macAddress[1],
               macAddress[2],
               macAddress[3],
               macAddress[4],
               macAddress[5]);
    UART_PRINT(lineBreak);
    UART_PRINT(lineBreak);
    UART_PRINT("\t");
    printBorder('=', 44);
    UART_PRINT(lineBreak);
    UART_PRINT(lineBreak);

    return(ret);
}

int system_init(void)
{
	int RetVal = -1;

    sl_WlanProfileDel(0xFF);
    RetVal = ConfigureSimpleLinkToDefaultState();
    if(RetVal < 0)
    {
        UART_PRINT("Network Terminal - Couldn't configure Network Processor\n");
        return(NULL);
    }
    RetVal = DisplayAppBanner(APPLICATION_NAME, APPLICATION_VERSION);

    if(RetVal < 0)
    {
        UART_PRINT("Unable to retrieve device information \n");
        return(NULL);
    }
    DisplayUsage();
    UART_PRINT("\r\nNetwork init start\r\n");

    InitializeAppVariables();
    Network_IF_provisionAP();

	return 0;
}
void system_reboot()
{
	/*
    PRCMMCUReset(TRUE);
    // PRCMSOCReset();
	*/

	// Another reboot method
	sl_WlanPolicySet(SL_POLICY_PM , SL_LOW_LATENCY_POLICY, NULL, 0);
//	sl_Stop(0);
	MAP_PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
	MAP_UtilsDelay(8000000);
	MAP_PRCMHibernateIntervalSet(330);
	MAP_PRCMHibernateEnter();
    while(1);

}
void system_reboot_1()
{

    PRCMMCUReset(TRUE);
}

int TcpClient(unsigned int uip, unsigned short  usPort)
{
    int             iCounter;
    int             iAddrSize;
    int             iSockID;
    int             iStatus;

    SlSockAddr_t	*sa;
    sockAddr_t 		sAddr;
    int32_t         nonBlocking;

    // filling the buffer
    for (iCounter=0 ; iCounter<BUFF_SIZE ; iCounter++)
    {
        g_cBsdBuf[iCounter] = (char)(iCounter % 10);
    }


    //filling the TCP server socket address
    sAddr.in4.sin_family = SL_AF_INET;
    sAddr.in4.sin_port = sl_Htons((unsigned short)usPort);
    sAddr.in4.sin_addr.s_addr = sl_Htonl((unsigned int)uip);

    sa = (SlSockAddr_t*)&sAddr.in4;
    iAddrSize = sizeof(SlSockAddrIn_t);



    // creating a TCP socket
    iSockID = sl_Socket(sa->sa_family, SL_SOCK_STREAM, 0);
    ASSERT_ON_ERROR(iSockID, SL_SOCKET_ERROR);

    if(TRUE == 1)
    {
        nonBlocking = TRUE;
        iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &nonBlocking, sizeof(nonBlocking));

        if(iStatus< 0)
        {
            UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus,
                       SL_SOCKET_ERROR);
            sl_Close(iSockID);
            return(-1);
        }
    }
    iStatus = -1;

    // connecting to TCP server
    while(iStatus < 0)
    {

    	iStatus = sl_Connect(iSockID, sa, iAddrSize);
        if((iStatus == SL_ERROR_BSD_EALREADY)&& (TRUE == 1))
        {
            sleep(1);
            continue;
        }
        else if(iStatus < 0)
        {
            UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus, SL_SOCKET_ERROR);
            sl_Close(iSockID);
            return(-1);
        }
        break;
    }


    int32_t buflen;
    uint32_t sent_bytes = 0;
    uint32_t bytes_to_send = (1000 * BUFF_SIZE);
    if(1)
    {
		while(sent_bytes < bytes_to_send)
		{
			if(bytes_to_send - sent_bytes >= BUFF_SIZE)
			{
				buflen = BUFF_SIZE;
			}
			else
			{
				buflen = bytes_to_send - sent_bytes;
			}

			/* Send packets to the server */
			iStatus = sl_Send(iSockID, g_cBsdBuf, buflen, 0);
		//            status = sl_Send(sock, &app_CB.CmdBuffer, buflen, 0);
			if((iStatus == SL_ERROR_BSD_EAGAIN) && (TRUE == 1))
			{
				sleep(1);
				continue;
			}
			else if(iStatus < 0)
			{
				UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus, SL_SOCKET_ERROR);
				sl_Close(iSockID);
				return(-1);
			}
			iCounter++;
			sent_bytes += iStatus;
		}
		UART_PRINT("Sent %u packets (%u bytes) (data:%s) successfully\n\r", iCounter, sent_bytes, g_cBsdBuf);
    }
    else
    {
    	uint32_t rcvd_bytes = 0;

		while(rcvd_bytes < (1000 * BUFF_SIZE))
		{
			iStatus = sl_Recv(iSockID, &g_cBsdBuf, MAX_BUF_SIZE, 0);
			if((iStatus == SL_ERROR_BSD_EAGAIN) && (TRUE == 1))
			{
				sleep(1);
				continue;
			}
			else if(iStatus < 0)
			{
				UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus, BSD_SOCKET_ERROR);
				sl_Close(iSockID);
				return(-1);
			}
			else if(iStatus == 0)
			{
				UART_PRINT("TCP Server closed the connection\n\r");
				break;
			}
			rcvd_bytes += iStatus;
		}

		UART_PRINT("Received %u packets (%u bytes) (data:%s) successfully\n\r",
				   (rcvd_bytes / BUFF_SIZE), rcvd_bytes, g_cBsdBuf);
    }

    return 0;
}

int TcpServer(unsigned short  usPort)
{

	int32_t             iCounter;
	int32_t             iAddrSize;
    int32_t				iSockID;
    int32_t            	iStatus;
    int32_t             iNewSockID = -1;
    int32_t         	lNonBlocking = FALSE;
    int32_t             iTestBufLen;

    SlSockAddr_t    *sa;
    SlSockAddr_t    *csa;
    sockAddr_t sAddr;

    // filling the buffer
    for (iCounter=0 ; iCounter<BUFF_SIZE ; iCounter++)
    {
        g_cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    iTestBufLen  = BUFF_SIZE;

    //filling the TCP server socket address
    sAddr.in4.sin_family = SL_AF_INET;

    /* Set the server's port:
	   We'll receive connection requests on this port */
    sAddr.in4.sin_port = sl_Htons((unsigned short)usPort);
    sAddr.in4.sin_addr.s_addr = SL_INADDR_ANY;

    sa = (SlSockAddr_t*)&sAddr.in4;
    csa = (SlSockAddr_t*)&sAddr.in4;
    iAddrSize = sizeof(SlSockAddrIn_t);
    // creating a TCP socket
    iSockID = sl_Socket(sa->sa_family, SL_SOCK_STREAM, 0);
    ASSERT_ON_ERROR(iSockID, SL_SOCKET_ERROR);



    // binding the TCP socket to the TCP server address
    iStatus = sl_Bind(iSockID, sa, iAddrSize);

    if( iStatus < 0 )
    {
        // error
        UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus,
                   SL_SOCKET_ERROR);
        sl_Close(iSockID);
        return(-1);
    }

    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(iSockID, 0);
    if( iStatus < 0 )
    {
        UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus,
                   SL_SOCKET_ERROR);
        sl_Close(iSockID);
        return(-1);
    }
    if(1)
    {
    	lNonBlocking = TRUE;

        iStatus = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING,
                                &lNonBlocking, sizeof(lNonBlocking));
        if(iStatus < 0)
        {
            UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus,
                       SL_SOCKET_ERROR);
            return(-1);
        }
        iNewSockID = SL_ERROR_BSD_EAGAIN;
    }

    // waiting for an incoming TCP connection
    while( iNewSockID < 0 )
    {
        // accepts a connection form a TCP client, if there is any
        // otherwise returns SL_EAGAIN
        iNewSockID = sl_Accept(iSockID, csa, (SlSocklen_t*)&iAddrSize);
        if( (iNewSockID == SL_ERROR_BSD_EAGAIN) && TRUE == 1)
        {
        	sleep(1);
        }
        else if( iNewSockID < 0 )
        {
            // error
        	UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus, SL_SOCKET_ERROR);
            sl_Close(iNewSockID);
            sl_Close(iSockID);
            return(-1);
        }
    }
    UART_PRINT("Connected to client: ");
    sAddr.in4.sin_addr.s_addr = sl_Htonl(sAddr.in4.sin_addr.s_addr);
    PrintIPAddress(0,(void*)&sAddr.in4.sin_addr);
    UART_PRINT("\n\r");
    // waits for 1000 packets from the connected TCP client
    uint32_t rcvd_bytes = 0;

   while(rcvd_bytes < (1000* BUFF_SIZE))
   {
        iStatus = sl_Recv(iNewSockID, g_cBsdBuf, iTestBufLen, 0);
        if((iStatus == SL_ERROR_BSD_EAGAIN) && (TRUE == 1))
        {
            sleep(1);
            continue;
        }
        else if(iStatus < 0)
        {
            UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, iStatus,
                       BSD_SOCKET_ERROR);
            sl_Close(iSockID);
            return(-1);
        }
        else if(iStatus == 0)
        {
            UART_PRINT("TCP Client closed the connection \n\r");
            break;
        }

        rcvd_bytes += iStatus;
    }

    Report("Recieved Data %s successfully\n\r",g_cBsdBuf);
    Report("Recieved %u packets successfully\n\r",g_ulPacketCount);

    // close the connected socket after receiving from connected TCP client
    iStatus = sl_Close(iNewSockID);
    ASSERT_ON_ERROR(iStatus, SL_SOCKET_ERROR);

    iStatus = sl_Close(iSockID);
    ASSERT_ON_ERROR(iStatus, SL_SOCKET_ERROR);

    return 0;
}
void PrintIPAddress(unsigned char ipv6,
                    void *ip)
{
    uint32_t        *pIPv4;
    uint8_t         *pIPv6;
    int32_t          i=0;

    if(!ip)
    {
        return;
    }

    if(ipv6)
    {
        pIPv6 = (uint8_t*) ip;

        for(i = 0; i < 14; i+=2)
        {
            UART_PRINT("%02x%02x:", pIPv6[i], pIPv6[i+1]);
        }

        UART_PRINT("%02x%02x", pIPv6[i], pIPv6[i+1]);
    }
    else
    {
        pIPv4 = (uint32_t*)ip;
        UART_PRINT("%d.%d.%d.%d",
                    SL_IPV4_BYTE(*pIPv4,3),
                    SL_IPV4_BYTE(*pIPv4,2),
                    SL_IPV4_BYTE(*pIPv4,1),
                    SL_IPV4_BYTE(*pIPv4,0));
    }
    return;
}
int32_t ipv4AddressParse(char *str, uint32_t *ipv4ip)
{
    volatile int32_t i = 0;
    uint32_t n;
    uint32_t ipv4Address = 0;
    char             *token;

    token = strtok(str, ".");
    if(token)
    {
        n = (int)strtoul(token, 0, 10);
    }
    else
    {
        return(-1);
    }

    while(i < 4)
    {
       if((token != NULL) && (n < 256))
       {
           ipv4Address |= n;
           if(i < 3)
           {
               ipv4Address = ipv4Address << 8;
           }
           token=strtok(NULL,".");
           if (token)
           {
               n = (int)strtoul(token, 0, 10);
           }
           i++;
       }
       else
       {
           return -1;
       }
    }
    *ipv4ip = ipv4Address;
/*    ipv4ip = &ipv4Address;
    UART_PRINT("ipv4ip parser:%x\n\r", *ipv4ip);*/
    return(0);
}
