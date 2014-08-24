DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += ws2812.c

CONTIKI_PROJECT = led_strip

all: $(CONTIKI_PROJECT) 

CONTIKI = ../../contiki
include $(CONTIKI)/Makefile.include


burn:

	cc-tool -v -e -w $(CONTIKI_PROJECT).hex
