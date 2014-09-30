#include <string.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/rpl/rpl.h"
#include "net/uip.h"

#include "one_wire.h"

#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "debug.h"
#include "net/uip-debug.h"

#include "one_wire.h"

static uint8_t onewire_id[8];

void onewire_init(void)
{
    unsigned int i;

    P2SEL &= ~(1<<0);
    P2DIR &= ~(1<<0);
    CLKCONCMD &= ~0x40;               /*Sets the system clock source for 32 MHZ crystal oscillator*/
    while(CLKCONSTA & 0x40);          /* Wait for Oscillator to be ready    */
    CLKCONCMD &= ~0x47;               /*Sets the system master clock frequency for 32 MHZ*/
    CLKCONCMD &= ~0x38;               /*Sets the Timer ticks clock frequency for 32 MHZ*/

    onewire_id_read();

    printf("DS18B20 ID :");
    for (i = 0; i < 8; i++)
    {
        printf("0x%x ", onewire_id[i]);
    }
    printf("\n");
    onewire_temp_read();
}

void onewire_id_get(uint8_t *buffer)
{
    int i;
    for (i = 0; i < 8; i++)
        buffer[i] = onewire_id[i];
}

void onewire_id_str_get(uint8_t *str)
{
    int i;

    for (i = 0; i < 8 ; ++i)
    {
        sprintf(str + (i << 1) , "%02X", onewire_id[i]);
    }
    str[16] = '\0';
}



uint16_t onewire_temp_read(void)
{
    uint8_t temp1,temp2;
    int16_t raw;

    onewire_reset();
    onewire_write(0xcc);
    onewire_write(0x44);

    onewire_reset();
    onewire_write(0xcc);
    onewire_write(0xbe);

    temp1 = onewire_read();
    temp2 = onewire_read();

    raw = (temp2 << 8) | temp1;
    return ((6 * raw) + raw / 4);

}

void onewire_reset(void)
{
    OUT_DQ();
    CLR_DQ();
    clock_delay_usec(480);
    SET_DQ();
    clock_delay_usec(60);
    IN_DQ();
    clock_delay_usec(10);
    while(!(GET_DQ()));
    OUT_DQ();
    SET_DQ();
}

uint8_t onewire_read(void)
{
    uint8_t data = 0;
    int i;

    for (i = 0; i < 8; i++)
    {
        OUT_DQ();
        CLR_DQ();
        data = data >> 1;
        SET_DQ();
        IN_DQ();
        clock_delay_usec(8);
        if(GET_DQ())
            data |= 0x80;
        OUT_DQ();
        SET_DQ();
        clock_delay_usec(60);
    }

    return data;
}


void onewire_write(uint8_t data)
{
    uint8_t i;

    OUT_DQ();
    SET_DQ();
    clock_delay_usec(16);

    for(i = 0; i < 8; i++)
    {
        CLR_DQ();
        if(data & 0x01)
        {
            SET_DQ();
        }
        else
        {
            CLR_DQ();
        }
        data = data >> 1;
        clock_delay_usec(40);  //65
        SET_DQ();
    }
}



void onewire_id_read(void)
{
    int i;

    onewire_reset();
    onewire_write(0x33);

    for(i = 0; i < 8; i++)
    {
        onewire_id[i] = onewire_read();
    }
}
