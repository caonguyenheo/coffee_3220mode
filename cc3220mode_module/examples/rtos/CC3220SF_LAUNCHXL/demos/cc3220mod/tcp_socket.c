#include <stdlib.h>
#include <stdint.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_types.h>

#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>

#include <ti/devices/cc32xx/driverlib/prcm.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/SPI.h>

#define APPLICATION_NAME        "TCP Socket"
#define APPLICATION_VERSION     "1.1.1"

#define IP_ADDR             0xc0a80064 /* 192.168.0.100 */
#define PORT_NUM            5001
#define BUF_SIZE            1400
#define TCP_PACKET_COUNT    1000


void * mainThread(void *arg)
{
	int32_t             RetVal ;
	pthread_attr_t      pAttrs_spawn;
	struct sched_param  priParam;
	struct timespec     ts = {0};

    SPI_init();
    InitTerm();

    DisplayBanner(APPLICATION_NAME, APPLICATION_VERSION);
	return(0);
}
