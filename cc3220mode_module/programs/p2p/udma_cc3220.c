/*
 * udma_cc3220.c
 *
 *  Created on: Sep 19, 2018
 *      Author: Admin
 */


#include "main_thread.h"
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/driverlib/udma.h>

#define MAX_NUM_CH              64  //32*2 entries
#define CTL_TBL_SIZE            64  //32*2 entries
#define UDMA_CH5_BITID          (1<<5)


tDMAControlTable gpCtlTbl[CTL_TBL_SIZE] __attribute__(( aligned(1024)));


void DmaSwIntHandler(void)
{
   MAP_uDMAIntClear(MAP_uDMAIntStatus());
}

void DmaErrorIntHandler(void)
{
    MAP_uDMAIntClear(MAP_uDMAIntStatus());
}

void UDMAInit()
{
    unsigned long ulNdx;

    //
    // Enable McASP at the PRCM module
    //
    MAP_PRCMPeripheralClkEnable(PRCM_UDMA,PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_UDMA);

    //
    // Register interrupt handlers
    //
    MAP_uDMAIntRegister(UDMA_INT_SW, DmaSwIntHandler);
    MAP_uDMAIntRegister(UDMA_INT_ERR, DmaErrorIntHandler);

    //
    // Enable uDMA using master enable
    //
    MAP_uDMAEnable();

    //
    // Set Control Table
    //
    for(ulNdx =0; ulNdx < sizeof(gpCtlTbl); ulNdx++)
    {
        *((unsigned char *)gpCtlTbl + ulNdx) = 0;
    }

    MAP_uDMAControlBaseSet(gpCtlTbl);
}

void UDMADeInit()
{
    //
    // UnRegister interrupt handlers
    //
    MAP_uDMAIntUnregister(UDMA_INT_SW);
    MAP_uDMAIntUnregister(UDMA_INT_ERR);

    //
    // Disable the uDMA
    //
    MAP_uDMADisable();
}
