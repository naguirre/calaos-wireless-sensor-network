#ifndef ONE_WIRE_H
#define ONE_WIRE_H


#include "contiki.h"
#include "contiki-conf.h"
#include "dev/leds.h"

#include "dev/button-sensor.h"
#include "dev/adc-sensor.h"

#include "sfr-bits.h"
#include "cc253x.h"


#define SET_DQ() P2_0 = 1;	 
#define CLR_DQ() P2_0 = 0;	
#define OUT_DQ() P2DIR |= (1<<0); 
#define IN_DQ()  P2DIR &= ~(1<<0);
#define GET_DQ() P2_0

#define DEBUG 1

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else /* DEBUG */
/* We overwrite (read as annihilate) all output functions here */
#define PRINTF(...)
#endif /* DEBUG */


void onewire_init(void);
void onewire_reset(void);
void onewire_id_get(uint8_t **id);
uint16_t onewire_temp_read(void);
void onewire_write(uint8_t data);
uint8_t onewire_read(void);
void onewire_id_read(void);

#endif
