/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,

 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_tirtos.c ========
 */
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include "Board.h"
#include "uart_terminal.h"
#include "main_thread.h"
#include "tcp.h"
#include "shelltask.h"

extern void * mainThread1(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE     1024
sem_t guserconfig_init;

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t 			thread;

    pthread_t           spawnThread;
    pthread_attr_t 		pAttrs;

    pthread_attr_t      attrs_spawn;
    struct 				sched_param priParam;
    int 				retc;
    int 				detachState;

    /* Call board init functions */
    Board_initGeneral();
    retc = sem_init(&guserconfig_init, 0, 0);
    if (retc < 0)
    {
        while (1);
    }
    /* Set priority and stack size attributes */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(retc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while(1)
        {
            ;
        }
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE * 4);
    if(retc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while(1)
        {
            ;
        }
    }

    /* create the sl_Task */
    pthread_attr_init(&attrs_spawn);
    priParam.sched_priority = 9;
    retc = pthread_attr_setschedparam(&attrs_spawn, &priParam);
    retc |= pthread_attr_setstacksize(&attrs_spawn, TASK_STACK_SIZE * 2);

    retc = pthread_create(&spawnThread, &attrs_spawn, sl_Task, NULL);
    if(retc)
    {
        /* Handle Error */
        UART_PRINT("Unable to create sl_Task thread\r\n");
        while(1);
    }

    priParam.sched_priority = 8;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    retc |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE * 2);

    if(retc != 0)
    {
        /* error handling */
        while(1)
        {
            UART_PRINT("Unable to create mainThread1 thread \n");
            return(NULL);
        }
    }
	retc = pthread_create(&thread, &pAttrs, mainThread1, NULL);
	if(retc != 0)
	{
		/* pthread_create() failed */
		while(1)
		{
			;
		}
	}

#if (ENABLE_TCP_TASK)
    //create thread TCP
//    pthread_attr_init(&pAttrs_tcp);
    priParam.sched_priority = TASK_PRIORITY_TCP;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    retc |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE * 4);

    if(retc != 0)
    {
        /* error handling */
        while(1)
        {
            UART_PRINT("Unable to create TCP thread \n");
            return(NULL);
        }
    }

    retc = pthread_create(&thread, &pAttrs, TcpThread, NULL);

    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }
#endif

#if (ENABLE_CLI_TASK)
    //create thread shell
//    pthread_attr_init(&pAttrs_shell);
    priParam.sched_priority = TASK_PRIORITY_CLI;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    retc |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE * 2);

    if(retc != 0)
    {
        /* error handling */
        while(1)
        {
            UART_PRINT("Unable to create Shell thread \n");
            return(NULL);
        }
    }

    retc = pthread_create(&thread, &pAttrs, ShellThread, NULL);

    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }

#endif
    BIOS_start();

    return (0);
}

/*
 *  ======== dummyOutput ========
 *  Dummy SysMin output function needed for benchmarks and size comparison
 *  of FreeRTOS and TI-RTOS solutions.
 */
void dummyOutput(void)
{
}
