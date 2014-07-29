#include "one_wire.h"





void init_one_wire(void){

  unsigned char *p,id[8];
  unsigned int i;

  P2SEL &= ~(1<<0);
  P2DIR &= ~(1<<0);
  CLKCONCMD &= ~0x40;               /*Sets the system clock source for 32 MHZ crystal oscillator*/
  while(CLKCONSTA & 0x40);          /* Wait for Oscillator to be ready    */
  CLKCONCMD &= ~0x47;               /*Sets the system master clock frequency for 32 MHZ*/
  CLKCONCMD &= ~0x38;               /*Sets the Timer ticks clock frequency for 32 MHZ*/
 
  /* // Timer 1 */
  /* T1CTL = 0x00; */


  /**----------------------Read ID------------------------*/
  p=readID();
  while(*p){id[i]=*p;p++;i++;}
  printf("DS18B20 ID :");
  i=0;
  while(i!=8){
    printf("0x%x ",id[i]);i++;
  }
  printf("\n\r");

  //  print_registers();

}




unsigned int read_temperature(void) 
{ 
    uint8_t temp1,temp2;

    reset_One_Wire(); 
    write_One_Wire(0xcc); 
    write_One_Wire(0x44); 

    reset_One_Wire(); 
    write_One_Wire(0xcc); 
    write_One_Wire(0xbe); 	

    temp1=read_One_Wire(); 
    temp2=read_One_Wire(); 
    return (unsigned int)(((temp2<<8)|temp1)*6.25); //0.0625=xx, 0.625=xx.x, 6.25=xx.xx
	
}






void print_registers(void){


  PRINTF("\nP0SEL:\t%X\n",P0SEL);
  PRINTF("P0DIR:\t%X\n",P0DIR);
  PRINTF("P1SEL:\t%X\n",P1SEL);
  PRINTF("P1DIR:\t%X\n",P1DIR);
  PRINTF("P2SEL:\t%X\n",P2SEL);
  PRINTF("P2DIR:\t%X\n\n",P2DIR);

  PRINTF("CLKCONCMD:\t%X\n",CLKCONCMD);
  PRINTF("CLKCONSTA:\t%X\n\n",CLKCONSTA);

  PRINTF("T1CTL:\t%X\n",T1CTL);
  PRINTF("T1CC0H:\t%X\n",T1CC0H);
  PRINTF("T1CC0L:\t%X\n",T1CC0L);
  PRINTF("T1STAT:\t%X\n\n",T1STAT);

}






void delay_us (uint16_t timeout) 
{
    while (timeout--)
    {
      __asm__("NOP");
      __asm__("NOP");
      __asm__("NOP");
    }
}




void delay_timer_us (uint16_t timeout)
{

  union {
    unsigned char byte[2];
    unsigned short val;
  } t1cnt;

  T1CNTL = 0x00; // set the count value to 0
  T1CTL = 0x01; // Modulo mode  

  // Wait until timer is above period to use
  do {
    t1cnt.byte[0] = T1CNTL;
    t1cnt.byte[1] = T1CNTH;
  } while (t1cnt.val <= (32*timeout)); // 1us = 32 * 31 ns ( with 32 MHz, 1 CLK = 31 ns )
  //  } while (t1cnt.val <= 0x0D06); // 1us = 32 * 31 ns ( with 32 MHz, 1 CLK = 31 ns )

  T1CTL = 0x00; // Operation is suspended

}



void reset_One_Wire(void) 
{
	OUT_DQ(); 
	CLR_DQ();
	delay_us(480);
	SET_DQ();
	delay_us(60);
	IN_DQ();
	delay_us(10);
	while(!(GET_DQ()));
	OUT_DQ();
	SET_DQ();
}




uint8_t read_One_Wire(void)
{
    uint8_t data=0,i=0;
    for(i=0;i<8;i++)
    {
	    OUT_DQ();
		CLR_DQ();
		data=data>>1;
		SET_DQ();
		IN_DQ()
		delay_us(8);
		if(GET_DQ())data|=0x80;
		OUT_DQ();
		SET_DQ();
		delay_us(60);
    }
    return(data);
}




void write_One_Wire(uint8_t data)
{
	uint8_t i=0;
	OUT_DQ();
	SET_DQ();
	delay_us(16);
    	for(i=0;i<8;i++)
    	{
	  CLR_DQ();

	  if(data&0x01){

	    SET_DQ();
	
	  }else{
	    CLR_DQ();
	  }

	  data=data>>1;
	  delay_us(40);  //65
	  SET_DQ();
	}
}



uint8_t* readID(void) 
{ 
    uint8_t ID[8],i,*p;
    reset_One_Wire(); 
    write_One_Wire(0x33);
    for(i=0;i<8;i++){
      ID[i]=read_One_Wire();
    }
    p=ID;
    return p;
}
