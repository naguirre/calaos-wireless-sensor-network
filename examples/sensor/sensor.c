#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/rpl/rpl.h"
#include "net/uip.h"

#include <string.h>
#include "dev/leds.h"
#include "dev/button-sensor.h"
//#include "debug.h"
#include "one_wire.h"

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"


#define SEND_INTERVAL    (8 * CLOCK_SECOND)

// FLAGS
#define DUP (1 << 7)
#define QoS (3 << 5)
#define RETAIN (1 << 4)
#define WILL (1 << 3)
#define CLEAN_SESSION (1 << 2)
#define TOPIC_ID_TYPE 3

PROCESS(mqtt_client_process, "Host mqtt-sn example");
//PROCESS_NAME(ping6_process);
AUTOSTART_PROCESSES(&mqtt_client_process);//,&ping6_process);




void timer_callback(void){

  static char buf[5];
  unsigned int tmp;
  uint8_t val, frac;

  tmp = onewire_temp_read();
  val = temp / 100;
  frac = temp % 100;
  
  sprintf(buf, "%d.%d", val, frac); 

  printf("\n\tTemperature:%d.%d\n", val, frac);

  //mqtt_publish(buf);

}


PROCESS_THREAD(mqtt_client_process, ev, data)
{

    static struct etimer sensors_timer;

    static uip_ip6addr_t server_address;
 
    // Setup an mqtt_connect_info_t structure to describe
    // how the connection should behave
   /* static mqtt_connect_info_t connect_info = */
   /*  { */
   /*      .client_id = "contiki", */
   /*      .buffer = NULL, */
   /*      .duration = 4, */
   /*      .flags = 1 << 5, //QoS level 1 */
   /*      .gwid = 0, */
   /*      .msg_id = 0, */
   /*      .protocol_id = 1, */
   /*      .radius = 0, */
   /*      .return_code = 0, */
   /*      .topic_id = 0, */
   /*      .topic_name = "temp", */
   /*      .will_message = NULL, */
   /*      .will_topic = NULL */
   /*  }; */
 
    // The list of topics that we want to subscribe to
    /* static const char* topics[64] = */
    /* { */
    /*   "led", "test", NULL */
    /* }; */


    /* const char* topic = NULL; */
    /* const char* message = NULL; */
    /* int level = 0; */

    PROCESS_BEGIN();
 

    printf("\nInit One Wire\n");
    onewire_init();

    // Set the server address
    /* uip_ip6addr(&server_address, */
    /*             0xbbbb, 0, 0, 0, 0, 0, 0, 0x0001); */
    uip_ip6addr(&server_address,
                0xbbbb, 0, 0, 0, 0, 0, 0, 0x001);
    /* uip_ip6addr(&server_address, */
    /*             0x2a01, 0x6600, 0x8081, 0x2c00, 0xd267, 0xe5ff, 0xfe43, 0x4c95); */


    printf("WAITING BUTTON\n");
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event);


    // Ask the client to connect to the server
    // and wait for it to complete.
    /* mqtt_connect(&server_address, UIP_HTONS(1883), */
    /*              1, &connect_info,topics); */


    etimer_set(&sensors_timer, SEND_INTERVAL);

    while(1)
    {

      PROCESS_YIELD();

      if(etimer_expired(&sensors_timer)){
	//handle_mqtt_output_udp();
	//if(sensors_start == 1){
	  timer_callback();
          //} 
	etimer_set(&sensors_timer, SEND_INTERVAL);
      }

    }

 
    PROCESS_END();

}
