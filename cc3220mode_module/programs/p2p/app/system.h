/*
 * system.h
 *
 *  Created on: Aug 30, 2018
 *      Author: Admin
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_


#include "main_thread.h"

#define PORT_NUM            5001
#define BUFF_SIZE            1400
#define TCP_PACKET_COUNT    1000
#define IP_ADDR             0xc0a80064

typedef enum
{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    SOCKET_CREATE_ERROR = -0x7D0,
    BIND_ERROR = SOCKET_CREATE_ERROR - 1,
    LISTEN_ERROR = BIND_ERROR -1,
    SOCKET_OPT_ERROR = LISTEN_ERROR -1,
    CONNECT_ERROR = SOCKET_OPT_ERROR -1,
    ACCEPT_ERROR = CONNECT_ERROR - 1,
    SEND_ERROR = ACCEPT_ERROR -1,
    RECV_ERROR = SEND_ERROR -1,
    SOCKET_CLOSE_ERROR = RECV_ERROR -1,
    DEVICE_NOT_IN_STATION_MODE_S = SOCKET_CLOSE_ERROR - 1,
    STATUS_CODE_MAX_S = -0xBB8
}e_AppStatusCodes;

typedef union
{
    uint32_t ipv4;          /* Ipv4 Address */
    uint8_t ipv6[16];       /* Ipv6 Address */
}ip_t;

typedef union
{
    SlSockAddrIn6_t in6;       /* Socket info for Ipv6 */
    SlSockAddrIn_t in4;        /* Socket info for Ipv4 */
}sockAddr_t;


int system_init(void);
void system_reboot();
void system_reboot_1();
void DisplayUsage();
void InitializeAppVariables(void);
int32_t ConfigureSimpleLinkToDefaultState();
void printBorder(char ch, int n);
int TcpClient(unsigned int uip, unsigned short  usPort);
int TcpServer(unsigned short  usPort);
void PrintIPAddress(unsigned char ipv6,void *ip);

int32_t ipv4AddressParse(char *str, uint32_t *ipv4ip);

#endif /* SYSTEM_H_ */
