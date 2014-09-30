DEFINES+=PROJECT_CONF_H=\"project-conf.h\"


# We need uIPv6, therefore we also need banking
HAVE_BANKING=1
UIP_CONF_IPV6=1
UIP_CONF_RPL=1

PROJECT_SOURCEFILES += mqtt-client.c mqtt-service.c one_wire.c

CONTIKI_PROJECT = sensor

#APPS = mqtt-service

all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki



include $(CONTIKI)/Makefile.include

burn:
	cc-tool -v -e -f -w $(CONTIKI_PROJECT).hex
