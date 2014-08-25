#include "spi.h"


void spi_init(void)
{
/*io initialize*/
     PERCFG |= 0x02;        // PERCFG.U1CFG = 1 
     P1SEL |= 0xE0;         // P1_7, P1_6, and P1_5 are peripherals 
//   P2INP &= ~(1<<7);
     P1SEL &= ~0x10;        // P1_4 is GPIO (SSN) 
     P1DIR |= 0x10;         // SSN is set as output 

     OUT_DC();
     OUT_RS();

/*SPI1 Configuration*/

     // Set baud rate to max (system clock frequency / 8) 
     // Assuming a 26 MHz crystal (CC1110Fx/CC2510Fx), 
     // max baud rate = 26 MHz / 8 = 3.25 MHz.  
     U1BAUD = 0x00;   // BAUD_M = 0 
     U1GCR |= 0x11;   // BAUD_E = 1
  
     // SPI Master Mode   
     U1CSR &= ~((1<<7)|(1<<5));
     U1CSR |= (1<<6);
  
     U1GCR |=(1<<7)|(1<<5)|(1<<6);
}


void spi_send_data(unsigned char tx_data)
{
     unsigned char rx_data;
     U1DBUF = tx_data; 
     while(0==(U1CSR&(1<<1)));
     U1CSR&=~((1<<1)|(1<<2));
     rx_data=U1DBUF;
}


unsigned char spi_receive_data(void)
{
     unsigned char rx_data;

     U1DBUF=0xff;
     while(0==(U1CSR&(1<<1)));
     rx_data=U1DBUF;
     U1CSR&=~((1<<1)|(1<<2));
 
     return rx_data;
}
