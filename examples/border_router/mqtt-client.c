#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/rpl/rpl.h"
#include "net/uip.h"

#include <string.h>
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "debug.h"
#include "one_wire.h"

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"

#include "mqtt-service.h"
 

PROCESS(mqtt_client_process, "Host mqtt-sn example");

AUTOSTART_PROCESSES(&mqtt_client_process);


PROCESS_THREAD(mqtt_client_process, ev, data)
{

    static uip_ip6addr_t server_address;


    PROCESS_BEGIN();

    /* Set the server address */
    uip_ip6addr(&server_address,
                0xbbbb, 0, 0, 0, 0, 0, 0, 0x001);

    /* connect to the broker */
    mqtt_connect(&server_address, UIP_HTONS(1883), 1000);


    while(1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == mqtt_event)
        {
            mqtt_event_data_t *event_data = data; 

            switch(event_data->type)
            {
            case MQTT_EVENT_TYPE_CONNECTED:
                printf("CONNECTED\n");                                
                //mqtt_subscribe("test");
                break;
            case MQTT_EVENT_TYPE_DISCONNECTED:
                printf("DISCONNECTED\n");
                break;
            case MQTT_EVENT_TYPE_SUBSCRIBED:
                printf("SUBSCRIBED\n");               
                break;
            case MQTT_EVENT_TYPE_UNSUBSCRIBED:
                printf("UNSUBSCRIBED\n");
                break;
            case MQTT_EVENT_TYPE_PUBLISHED:
                printf("PUBLISHED\n");
                break;
            case MQTT_EVENT_TYPE_PUBLISH:
                printf("PUBLISH %s\n", event_data->data);
                break;
            default:
                break;
            }
        }
        
    }

 
    PROCESS_END();

}

