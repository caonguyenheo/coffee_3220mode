/*
 * spi_handler.c
 *
 *  Created on: Sep 18, 2018
 *      Author: CNN
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


#include "uart_terminal.h"
#include "main_thread.h"
#include "udma_cc3220.h"
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/driverlib/udma.h>
#include <ti/display/Display.h>



#define SPI_20_MHZ              200000000


#define SPI_MSG_LENGTH  (30)
#define SLAVE_MSG       ("Hello from slave, msg#: ")

#define MAX_LOOP        (10)

unsigned char slaveRxBuffer[SPI_MSG_LENGTH];
unsigned char slaveTxBuffer[SPI_MSG_LENGTH];

#define SPI_MSG_LENGTH  (30)
#define MASTER_MSG      ("Hello from master, msg#: ")

#define MAX_LOOP        (10)

//static Display_Handle display;

unsigned char masterRxBuffer[SPI_MSG_LENGTH];
unsigned char masterTxBuffer[SPI_MSG_LENGTH];

/* Semaphore to block master until slave is ready for transfer */
extern sem_t slaveSem;
extern sem_t masterSem;
//sem_t slaveSem;
//sem_t masterSem;




void transferCompleteFxn(SPI_Handle handle, SPI_Transaction *transaction)
{
    sem_post(&slaveSem);
}

void slaveReadyFxn(uint_least8_t index)
{
    sem_post(&masterSem);
}


void spi_slave()
{
    SPI_Handle      slaveSpi;
    SPI_Params      spiParams;
    SPI_Transaction transaction;
    uint32_t        i;
    bool            transferOK;

    int status;

    GPIO_setConfig(Board_SPI_SLAVE_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_SPI_MASTER_READY, GPIO_CFG_INPUT);

    GPIO_write(Board_SPI_SLAVE_READY, 1);
    while (GPIO_read(Board_SPI_MASTER_READY) == 0);

/*    status  = sem_init(&slaveSem, 0, 0);

    if (status  != 0)
    {
    	UART_PRINT("Error creating slaveSem line [%d] function [%d]\n\r",__LINE__, __FUNCTION__);

        while(1);
    }*/
    while (GPIO_read(Board_SPI_MASTER_READY));


    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.mode = SPI_SLAVE;
    spiParams.bitRate = SPI_20_MHZ;
    spiParams.dataSize = 8;
    spiParams.transferCallbackFxn = transferCompleteFxn;
    spiParams.transferMode = SPI_MODE_CALLBACK;
    slaveSpi = SPI_open(Board_SPI_SLAVE, &spiParams);
    if (slaveSpi == NULL)
    {
//    	Display_printf(display, 0, 0, "Error initializing slave SPI\n\r");
    	UART_PRINT("Error initializing slave SPI\n\r");
        while (1);
    }
    else
    {
    	UART_PRINT("Slave SPI initialized\n\r");
    }

    strncpy((char *) slaveTxBuffer, SLAVE_MSG, SPI_MSG_LENGTH);

    //    for (i = 0; i < MAX_LOOP; i++) {
	while (1)
	{
         /* Initialize slave SPI transaction structure */
		slaveTxBuffer[sizeof(SLAVE_MSG) - 1] = (i % 10) + '0';
		memset((void *) slaveRxBuffer, 0, SPI_MSG_LENGTH);
		transaction.count = SPI_MSG_LENGTH;
		transaction.txBuf = (void *) slaveTxBuffer;
		transaction.rxBuf = (void *) slaveRxBuffer;

         GPIO_toggle(Board_GPIO_LED1);

		transferOK = SPI_transfer(slaveSpi, &transaction);
		if (transferOK)
		{
			GPIO_write(Board_SPI_SLAVE_READY, 0);
			sem_wait(&slaveSem);
			GPIO_write(Board_SPI_SLAVE_READY, 1);
			UART_PRINT("Slave received: %s\n\r", slaveRxBuffer);
		}
		else
		{
			UART_PRINT("Unsuccessful slave SPI transfer\n\r");
		}
	}

	SPI_close(slaveSpi);

	GPIO_setConfig(Board_SPI_MASTER_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
	GPIO_write(Board_SPI_SLAVE_READY, 0);

	UART_PRINT("Done\n\r");

	return (NULL);
}

void spi_master()
{
    SPI_Handle      masterSpi;
    SPI_Params      spiParams;
    SPI_Transaction transaction;
    uint32_t        i;
    bool            transferOK;
    int32_t         status;


    GPIO_setConfig(Board_SPI_MASTER_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_SPI_SLAVE_READY, GPIO_CFG_INPUT);


    GPIO_write(Board_SPI_MASTER_READY, 1);
    while (GPIO_read(Board_SPI_SLAVE_READY) == 0) {}


    GPIO_setConfig(Board_SPI_SLAVE_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(Board_SPI_SLAVE_READY, slaveReadyFxn);
    GPIO_enableInt(Board_SPI_SLAVE_READY);



/*    status = sem_init(&masterSem, 0, 0);
    if (status!= 0)
    {
    	UART_PRINT("Error creating masterSem line [%d] function [%d]\n\r",__LINE__, __FUNCTION__);
        while(1);
    }*/
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.bitRate = SPI_20_MHZ;
    spiParams.dataSize = 8;
    masterSpi = SPI_open(Board_SPI_MASTER, &spiParams);
    if (masterSpi == NULL)
    {
    	UART_PRINT("Error initializing master SPI\n\r");
        while (1);
    }
    else
    {
    	UART_PRINT("Master SPI initialized\n\r");
    }

    GPIO_write(Board_SPI_MASTER_READY, 0);

    strncpy((char *) masterTxBuffer, MASTER_MSG, SPI_MSG_LENGTH);

//    for (i = 0; i < MAX_LOOP; i++) {
    while (1)
    {

        sem_wait(&masterSem);

        /* Initialize master SPI transaction structure */
        masterTxBuffer[sizeof(MASTER_MSG) - 1] = (i % 10) + '0';
        memset((void *) masterRxBuffer, 0, SPI_MSG_LENGTH);
        transaction.count = SPI_MSG_LENGTH;
        transaction.txBuf = (void *) masterTxBuffer;
        transaction.rxBuf = (void *) masterRxBuffer;

        GPIO_toggle(Board_GPIO_LED1);


        transferOK = SPI_transfer(masterSpi, &transaction);
        if (transferOK == 1)
        {
        	UART_PRINT("Master received: %s\n\r", masterRxBuffer);
        }
        else
        {
        	UART_PRINT("Unsuccessful master SPI transfer\n\r");
        }

        sleep(3);
    }

    SPI_close(masterSpi);

    GPIO_disableInt(Board_SPI_SLAVE_READY);
    GPIO_setConfig(Board_SPI_SLAVE_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_write(Board_SPI_MASTER_READY, 0);

    UART_PRINT("Done\n\r");

    return (NULL);
}

void * spiSlaveHandleTask(void * param)
{
//	Display_init();

#if 0
	spi_slave();
#else
	spi_master();
#endif

	return(0);
}
