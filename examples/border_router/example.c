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
 

#define SEND_INTERVAL    (8 * CLOCK_SECOND)

// FLAGS
#define DUP (1 << 7)
#define QoS (3 << 5)
#define RETAIN (1 << 4)
#define WILL (1 << 3)
#define CLEAN_SESSION (1 << 2)
#define TOPIC_ID_TYPE 3

PROCESS(mqtt_client_process, "Host mqtt-sn example");
PROCESS(neighbor_detection, "Neighbor detection");
//PROCESS_NAME(ping6_process);
AUTOSTART_PROCESSES(&mqtt_client_process);//,&ping6_process);


process_event_t neighbor_discovery_event;
struct process* calling_process;


void timer_callback(void){

  char buf[6];
  unsigned int tmp;

  tmp = read_temperature();
  buf[0] = ((tmp/100)/10)+48;
  buf[1] = ((tmp/100)-(((tmp/100)/10)*10)) + 48;
  buf[2] = '.';
  buf[3] = ((tmp%100)/10)+48;
  buf[4] = ((tmp%100)-(((tmp%100)/10)*10)) + 48;
  buf[5] = '\0';
  printf("\n\tTemperature:%c%c%c%c%c%c\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);

  mqtt_publish(buf);

}



static void
get_mac_addr(char* buffer)
{
  int i = 0, j = 0;

  for(i = 0; i < UIP_802154_LONGADDR_LEN; i++){
    buffer[j] = decimal_hex(uip_lladdr.addr[i]/16);
    buffer[j+1] = decimal_hex(uip_lladdr.addr[i]);
    j = j + 2;
  }
  buffer[1] = '0'; //look address ipv6 format
  buffer[16] = '\0';
}



static void
get_topic_name(char* buffer, char* id, char* id_ow)
{
  memcpy(buffer, id, strlen(id));
  memcpy(buffer + strlen(id), "/ONEWIRE/", strlen("/ONEWIRE/"));
  memcpy(buffer + strlen(id) + strlen("/ONEWIRE/"),  id_ow, strlen(id_ow));
}

PROCESS_THREAD(mqtt_client_process, ev, data)
{

    static struct etimer sensors_timer;
    static struct etimer connected_timer;
    static struct sensors_sensor *sensor;

    static uip_ip6addr_t server_address;
 
    // Setup an mqtt_connect_info_t structure to describe
    // how the connection should behave
   static mqtt_connect_info_t connect_info =
    {
       .client_id = NULL,
       .buffer = NULL,
       .duration = 4,
       .flags = 1 << 5, //QoS level 1
       .gwid = 0,
       .msg_id = 0,
       .protocol_id = 1,
       .radius = 0,
       .return_code = 0,
       .topic_id = 0,
       .topic_name = NULL,
       .will_message = NULL,
       .will_topic = NULL
    };
 
    // The list of topics that we want to subscribe to
    static const char* topics[64] =
    {
      /* "led",  NULL */
      "OUT_DIGITAL/1", "OUT_DIGITAL/2", "OUT_DIGITAL/3", NULL
    };


    static char id[17];
    static char topic_name[64];
    static unsigned char id_ow[8];

    PROCESS_BEGIN();

    // wait neighbor discovery event
    neighbor_discovery_event = process_alloc_event();
    calling_process = PROCESS_CURRENT();
    process_start(&neighbor_detection, NULL);
    printf("WAITING Neighbor Discovery Event\n");
    PROCESS_WAIT_EVENT_UNTIL(ev == neighbor_discovery_event);
    leds_on( LEDS_RED | LEDS_YELLOW | LEDS_GREEN );

    // set client id
    memset(id, 0, sizeof(id));
    get_mac_addr(id);
    connect_info.client_id = id;

    // init one wire
    printf("\nInit One Wire\n");
    init_one_wire();

    // set topic name
    memset(id_ow, 0, sizeof(id_ow));
    get_id_ow(id_ow);
    memset(topic_name, 0, sizeof(topic_name));
    get_topic_name(topic_name,id,id_ow);
    connect_info.topic_name = topic_name;

    // Set the server address
    uip_ip6addr(&server_address,
                0xbbbb, 0, 0, 0, 0, 0, 0, 0x001);

    // connect to the broker
    mqtt_connect(&server_address, UIP_HTONS(1883),
                 1, &connect_info,topics);


    etimer_set(&connected_timer, 10 * CLOCK_SECOND);
    etimer_set(&sensors_timer, SEND_INTERVAL);

    while(1)
    {

      PROCESS_YIELD();

      if(ev == tcpip_event){
	tcpip_handler();	
      }
      else if(etimer_expired(&connected_timer) && (connected == 0)){
	  printf("NOT CONNECTED\n");
      	  send_connect_msg();
	  etimer_set(&connected_timer,  10 * CLOCK_SECOND);
      }
      else if(etimer_expired(&sensors_timer)){
	handle_mqtt_output_udp();
	if(sensors_start == 1){
	  timer_callback();
	} 
	etimer_set(&sensors_timer, SEND_INTERVAL);
      }else if(ev == sensors_event){
	sensor = (struct sensors_sensor *)data;
	if(sensor == &button_1_sensor) {
	  printf("BUTTON 1 pressed\n");
	  publish_first_switch("1");
	}else if(sensor == &button_2_sensor){
	  printf("BUTTON 2 pressed\n");
	  publish_second_switch("1");
	}
      }

    }

 
    PROCESS_END();

}



PROCESS_THREAD(neighbor_detection, ev, data)
{

    static struct etimer neighbor_timer;
    static struct neighbor_addr *item;

    PROCESS_BEGIN();

    etimer_set(&neighbor_timer, 4 * CLOCK_SECOND);

    while(1)
    {

	PROCESS_YIELD();

	item = neighbor_attr_list_neighbors();
	/* printf("\nNEIGHBOR value: %p\n\n\n",item); */
	if(item != NULL){
	    process_post_synch(calling_process, neighbor_discovery_event, NULL);
	    process_exit(&neighbor_detection);
	}else{
	    etimer_set(&neighbor_timer, 4 * CLOCK_SECOND);
	}

    }

    PROCESS_END();

}
