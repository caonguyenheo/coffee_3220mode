/*
 * system.c
 *
 *  Created on: Aug 30, 2018
 *      Author: Admin
 */


#include "main_thread.h"
#include "network_handler.h"
#include "system.h"

char lineBreak[]                = "\n\r";
void printBorder(char ch, int n)
{
    int        i = 0;

    for(i=0; i<n; i++)    putch(ch);
}
void DisplayUsage()
{
	UART_PRINT("\t=========================================================================================\r\n");
	UART_PRINT("\t\tAT COMMAND SUPPORT:\r\n");
    UART_PRINT("\tat-wificonnect [SSID] [PASSWD]\t");
    UART_PRINT("\tat-disconnect\t");
    UART_PRINT("\tat-reboot\n\r");
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

    UART_PRINT("\r\nNetwork init start\r\n");

    InitializeAppVariables();
    Network_IF_provisionAP();
//    DisplayUsage();
	return 0;
}
void system_reboot()
{
	/*
    PRCMMCUReset(TRUE);
    // PRCMSOCReset();
	*/

	// Another reboot method
//	sl_WlanPolicySet(SL_POLICY_PM , SL_LOW_LATENCY_POLICY, NULL, 0);
	sl_Stop(0);
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
