/*
 * main_thread.c
 *
 *  Created on: Aug 29, 2018
 *      Author: Admin
 */
#include "main_thread.h"
#include "tcp.h"
#include "shelltask.h"
#include "system.h"


pthread_t           gSpawn_thread = (pthread_t)NULL;
pthread_t           gShell_thread = (pthread_t)NULL;
pthread_t           gTcp_thread = (pthread_t)NULL;
appControlBlock     app_CB;

//int32_t ConfigureSimpleLinkToDefaultState()
//{
//    uint8_t                              ucConfigOpt = 0;
//    uint8_t                              ucConfigLen = 0;
//    uint8_t                              ucPower     = 0;
//    int32_t                              RetVal = -1;
//    int32_t                              Mode = -1;
//    uint32_t                             IfBitmap = 0;
//    SlWlanScanParamCommand_t             ScanDefault = {0};
//    SlWlanRxFilterOperationCommandBuff_t RxFilterIdMask = {{0}};
//    SlDeviceVersion_t      ver = {{0}};
//
//    Mode = sl_Start(0, 0, 0);
//    ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
//    if(Mode != ROLE_STA)
//    {
//          /* Set NWP role as STA */
//          Mode = sl_WlanSetMode(ROLE_STA);
//          ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
//
//        /* For changes to take affect, we restart the NWP */
//        RetVal = sl_Stop(SL_STOP_TIMEOUT);
//        ASSERT_ON_ERROR(RetVal, DEVICE_ERROR);
//
//        Mode = sl_Start(0, 0, 0);
//        ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
//    }
//    if(Mode != ROLE_STA)
//    {
//        UART_PRINT("Failed to configure device to it's default state");
//        return -1;
//    }
//    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
//    ucConfigLen = sizeof(ver);
//    RetVal = sl_DeviceGet(SL_DEVICE_GENERAL, &ucConfigOpt,
//                                &ucConfigLen, (_u8 *)(&ver));
//    UART_PRINT("Host Driver Version: %s\r\n", SL_DRIVER_VERSION);
//    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\r\n",
//    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
//    ver.FwVersion[0],ver.FwVersion[1],
//    ver.FwVersion[2],ver.FwVersion[3],
//    ver.PhyVersion[0],ver.PhyVersion[1],
//    ver.PhyVersion[2],ver.PhyVersion[3]);
//
//    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION, SL_WLAN_CONNECTION_POLICY(1,0,0,0), NULL ,0);
////    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,SL_WLAN_CONNECTION_POLICY(1,1,0,1),NULL,0);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//    RetVal = sl_WlanProfileDel(0xFF);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//   RetVal = sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP , 1, 1);//SL_NETCFG_IPV4_STA_ADDR_MODE
//   ASSERT_ON_ERROR(RetVal, NETAPP_ERROR);
//    /* Disable ipv6 */
//    IfBitmap = !(SL_NETCFG_IF_IPV6_STA_LOCAL | SL_NETCFG_IF_IPV6_STA_GLOBAL);
//    RetVal = sl_NetCfgSet(SL_NETCFG_IF, SL_NETCFG_IF_STATE, sizeof(IfBitmap),(const unsigned char *)&IfBitmap);
//    ASSERT_ON_ERROR(RetVal, NETAPP_ERROR);
//
//
////    /* Configure scan parameters to default */
////    ScanDefault.ChannelsMask = CHANNEL_MASK_ALL;
////    ScanDefault.RssiThershold = RSSI_TH_MAX;
////
////    RetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, SL_WLAN_GENERAL_PARAM_OPT_SCAN_PARAMS, sizeof(ScanDefault), (uint8_t *)&ScanDefault);
////    ASSERT_ON_ERROR(RetVal);
//
//    // Disable scan
//    ucConfigOpt = SL_WLAN_SCAN_POLICY(0, 0);
//    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_SCAN ,  ucConfigOpt, NULL, 0);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//    // Set TX power lvl to max
//    ucPower = 0;
//    RetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, SL_WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (_u8 *)&ucPower);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//    // Set NWP Power policy to 'normal'
//    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_PM, SL_WLAN_NORMAL_POLICY, NULL, 0);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//    // Unregister mDNS services
//    RetVal = sl_NetAppMDNSUnRegisterService(0, 0, 0);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//    /* Remove all 64 RX filters (8*8) */
//    memset(RxFilterIdMask.FilterBitmap, 0xFF, 8);
//    RetVal = sl_WlanSet(SL_WLAN_RX_FILTERS_ID,
//    		   SL_WLAN_RX_FILTER_REMOVE,
//        	   sizeof(SlWlanRxFilterOperationCommandBuff_t),
//    		   (_u8 *)&RxFilterIdMask);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//    RetVal = sl_Stop(SL_STOP_TIMEOUT);
//    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//    InitializeAppVariables();
//	 return 0;
//
//}


int32_t    initAppVariables(void)
{
    int32_t ret = 0;

    app_CB.Status = 0 ;
    app_CB.Role = ROLE_RESERVED;
    app_CB.Exit = FALSE;

    memset(&app_CB.CmdBuffer, 0x0, CMD_BUFFER_LEN);
    memset(&app_CB.gDataBuffer, 0x0, sizeof(app_CB.gDataBuffer));
    memset(&app_CB.CON_CB, 0x0, sizeof(app_CB.CON_CB));

    ret = sem_init(&app_CB.CON_CB.connectEventSyncObj,    0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    ret = sem_init(&app_CB.CON_CB.eventCompletedSyncObj,  0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    ret = sem_init(&app_CB.CON_CB.ip4acquireEventSyncObj, 0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    ret = sem_init(&app_CB.CON_CB.ip6acquireEventSyncObj, 0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    memset(&app_CB.P2P_CB, 0x0, sizeof(app_CB.P2P_CB));

    ret = sem_init(&app_CB.P2P_CB.DeviceFound, 0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    ret = sem_init(&app_CB.P2P_CB.RcvConReq, 0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    ret = sem_init(&app_CB.P2P_CB.RcvNegReq, 0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    ret = sem_init(&app_CB.WowlanSleepSem, 0, 0);
    if(ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return(-1);
    }

    return(ret);
}


void * mainThread(void *arg)
{
    int32_t             RetVal ;
    pthread_attr_t      pAttrs_spawn;
    pthread_attr_t      pAttrs_shell;
    pthread_attr_t      pAttrs_tcp;
    struct sched_param  priParam;
    struct timespec     ts = {0};

    /* Initializes the SPI interface to the Network
       Processor and peripheral SPI (if defined in the board file) */
    SPI_init();
    GPIO_init();

    /* Init Application variables */
//    RetVal = initAppVariables();

    /* Init Terminal UART */
    InitTerm();

    /* initialize the realtime clock */
    clock_settime(CLOCK_REALTIME, &ts);

    /* Switch off all LEDs on boards */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED2, Board_GPIO_LED_OFF);

    /* Create the sl_Task internal spawn thread */
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = SPAWN_TASK_PRIORITY;
    RetVal = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs_spawn, TASK_STACK_SIZE * 2);

    /* The SimpleLink host driver architecture mandate spawn
       thread to be created prior to calling Sl_start (turning the NWP on). */
    /* The purpose of this thread is to handle
       asynchronous events sent from the NWP.
     * Every event is classified and later handled
       by the Host driver event handlers. */
    RetVal = pthread_create(&gSpawn_thread, &pAttrs_spawn, sl_Task, NULL);
    if(RetVal < 0)
    {
        /* Handle Error */
        UART_PRINT("Network Terminal - Unable to create spawn thread \n");
        return(NULL);
    }


    //create thread TCP
//    system_init();
    pthread_attr_init(&pAttrs_tcp);
    priParam.sched_priority = TASK_PRIORITY_TCP;
    RetVal = pthread_attr_setschedparam(&pAttrs_tcp, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs_tcp, TASK_STACK_SIZE * 4);

    if(RetVal != 0)
    {
        // error handling
        while(1)
        {
            UART_PRINT("Unable to create TCP thread \n");
            return(NULL);
        }
    }

    RetVal = pthread_create(&gTcp_thread, &pAttrs_tcp, TcpTask, NULL);

    if(RetVal != 0)
    {
        while(1)
        {
            ;
        }
    }

    //create thread shell
    pthread_attr_init(&pAttrs_shell);
    priParam.sched_priority = TASK_PRIORITY_CLI;
    RetVal = pthread_attr_setschedparam(&pAttrs_shell, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs_shell, TASK_STACK_SIZE * 3);

    if(RetVal != 0)
    {
        /* error handling */
        while(1)
        {
            ;
        }
    }

    RetVal = pthread_create(&gShell_thread, &pAttrs_shell, ShellTask, NULL);

    if(RetVal != 0)
    {
        while(1)
        {
            ;
        }
    }


    return(0);
}
