DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
DEFINES+=CC_CONF_STACK_FRIENDLY=1
HAVE_BANKING=1
UIP_CONF_IPV6=1
UIP_CONF_RPL=1
RF_CHANNEL=26

PROJECT_SOURCEFILES += ping6.c one_wire.c


# End of user configuration section

CONTIKI_PROJECT = sensor

all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki/

include $(CONTIKI)/Makefile.include

burn:
	cc-tool -v -e -w $(CONTIKI_PROJECT).hex
