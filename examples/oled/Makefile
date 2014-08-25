DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += spi.c draw.c one_wire.c

CONTIKI_PROJECT = oled-temp

all: $(CONTIKI_PROJECT) 

CONTIKI = ../../contiki
include $(CONTIKI)/Makefile.include


burn:

	cc-tool -v -e -w $(CONTIKI_PROJECT).hex
