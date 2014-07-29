DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

HAVE_BANKING=1
UIP_CONF_IPV6=1
UIP_CONF_RPL=1
RF_CHANNEL=26
# End of user configuration section

CONTIKI_PROJECT = button_test

all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki/

include $(CONTIKI)/Makefile.include

burn:
	cc-tool -v -e -w $(CONTIKI_PROJECT).hex
