/*
 * spi_handler.h
 *
 *  Created on: Sep 18, 2018
 *      Author: CNN
 */

#ifndef SPI_HANDLER_H_
#define SPI_HANDLER_H_

enum SPI_PACKET_TYPE
{
	CMD_VERSION = 1,
	DATA_STREAMING = 3,
	MAX_PACKET_TYPE
};


void * spiSlaveHandleTask(void * param);
void spi_init();
void spi_deinit(void);

#endif /* SPI_HANDLER_H_ */
