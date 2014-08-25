
#define SET_RS() P1_2 = 1;
#define CLR_RS() P1_2 = 0;

static void _spi_init(void)
{
  PERCFG |= 0x02;
  P1SEL |= 0xE0;
  P1SEL &= ~0x10;
  P1DIR |= 0x10;
  P1DIR |= 0x08;
  

}

void screen_init(void)
{
  _spi_init();
  SET_RS();
  clock_delay(1000);
  CLR_RS();
  clock_delay(10000);
  SET_RS();
}

void screen_blank(void)
{
}

void screen_on(void)
{
}

void screen_on(void)
{
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}
