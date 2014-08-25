#ifndef __SPI_H__
#define __SPI_H__

#include "sfr-bits.h"
#include "cc253x.h"

#define SET_CS() P1_4 = 1;
#define CLR_CS() P1_4 = 0;
#define SET_DC() P1_3 = 1;
#define CLR_DC() P1_3 = 0;
#define OUT_DC() P1DIR |= 0x08;
#define SET_RS() P1_2 = 1;
#define CLR_RS() P1_2 = 0;
#define OUT_RS() P1DIR |= 0x04;

void spi_init(void);

void spi_send_data(unsigned char tx_data);

unsigned char spi_receive_data(void);

#endif
