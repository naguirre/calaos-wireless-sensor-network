/*
 * Copyright (c) 2014, Stephen Robinson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 *  are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "contiki.h"
#include "contiki-net.h"
#include "sys/etimer.h"

#include <string.h>
#include <stdio.h>

#include "dev/leds.h"
#include "mqtt-service.h"

#include "debug.h"

#define DEBUG DEBUG_FULL
#include "net/ip/uip-debug.h"



typedef struct mqtt_state_t
{
  uip_ip6addr_t address;
  uint16_t port;
  struct uip_udp_conn* udp_connection;

  mqtt_connect_info_t* connect_info;

  int pending_msg_type;
  char* data; //data can be a topic, dest, data ...

  const char* topics[64];
} mqtt_state_t;



mqtt_state_t mqtt_state;
mqtt_message_t message;

int sensors_start = 0;


/*********************************************************************
*
*    Public API
*
*********************************************************************/

static print_topics( const char* topics[]){
  int i = 0;
  for(i = 0; topics[i] != NULL; i++){
    printf(" Subscribe to TOPIC: %s\n",topics[i]);
  }
}

// Connect to the specified server
void mqtt_connect(uip_ip6addr_t* address, uint16_t port, int auto_reconnect, mqtt_connect_info_t* info, char* topics[])
{
int i = 0;

  // init mqtt_state
  uip_ipaddr_copy(&mqtt_state.address, address);
  mqtt_state.port = port;
  mqtt_state.connect_info = info;
  mqtt_state.connect_info->buffer = &message;
  mqtt_state.pending_msg_type = NULL;
  mqtt_state.data = NULL;

  for(i = 0; topics[i] != NULL; i++){
      mqtt_state.topics[i] = topics[i];
  }
  print_topics(mqtt_state.topics);

  // connect to the server
  printf("mqtt: connecting...\n");
  mqtt_state.udp_connection = udp_new(&mqtt_state.address, mqtt_state.port, NULL);
  if ( mqtt_state.udp_connection != NULL) {
    PRINTF("Created a connection with the server ");
    PRINT6ADDR(&mqtt_state.udp_connection->ripaddr);
    PRINTF(" local/remote port %u/%u\n",
	   UIP_HTONS(mqtt_state.udp_connection->lport), UIP_HTONS(mqtt_state.udp_connection->rport));
  } else {
    PRINTF("Could not open connection\n");
  }

  // connect to broker
  mqtt_msg_connect(mqtt_state.connect_info);
  uip_udp_packet_send(mqtt_state.udp_connection, mqtt_state.connect_info->buffer->data, mqtt_state.connect_info->buffer->length);

}


void mqtt_advertise(void)
{
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_ADVERTISE;
}

void mqtt_searchgw(void)
{
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_SEARCHGW;
}


void mqtt_willtopic(char* willtopic)
{
  memcpy(mqtt_state.connect_info->will_topic, willtopic, strlen(willtopic));
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_WILLTOPIC;
}


void mqtt_willmsg(char* willmsg)
{
  memcpy(mqtt_state.connect_info->will_message, willmsg, strlen(willmsg));
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_WILLMSG;
}



void mqtt_register(void)
{
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_REGISTER;
}



void mqtt_publish(char* data)
{
  mqtt_state.data = data;
  mqtt_msg_publish(mqtt_state.connect_info, data);
  uip_udp_packet_send(mqtt_state.udp_connection, mqtt_state.connect_info->buffer->data, mqtt_state.connect_info->buffer->length);
}


void mqtt_subscribe_name(char* topic)
{
  mqtt_state.data = "name";
  mqtt_state.connect_info->topic_name = topic;
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_SUBSCRIBE;
}


void mqtt_subscribe_id(uint16_t id)
{
  mqtt_state.data = "id";
  mqtt_state.connect_info->topic_id = id;
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_SUBSCRIBE;
}


void mqtt_unsubscribe_name(char* topic)
{
  mqtt_state.data = "name";
  mqtt_state.connect_info->topic_name = topic;
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_UNSUBSCRIBE;
}


void mqtt_unsubscribe_id(uint16_t id)
{
  mqtt_state.data = "id";
  mqtt_state.connect_info->topic_id = id;
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_UNSUBSCRIBE;
}


void mqtt_pingreq(char* dest){
  mqtt_state.data = dest;
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_PINGREQ;
}


void mqtt_pingresp(void){
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_PINGRESP;
}


void mqtt_disconnect(void)
{
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_DISCONNECT;
}


void mqtt_willtopicupd(char* willtopic)
{
  memcpy(mqtt_state.connect_info->will_topic, willtopic, strlen(willtopic));
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_WILLTOPICUPD;
}


void mqtt_willmsgupd(char* willmsg)
{
  memcpy(mqtt_state.connect_info->will_message, willmsg, strlen(willmsg));
  mqtt_state.pending_msg_type = MQTT_MSG_TYPE_WILLMSGUPD;
}


static handle_publish_msg(char* data)
{

  if(strcmp(data + 7, "yellow")){
      leds_on(LEDS_YELLOW);
      leds_off(LEDS_RED | LEDS_GREEN);
  }else if(strcmp(data + 7, "red")){
      leds_on(LEDS_RED);
      leds_off(LEDS_YELLOW | LEDS_GREEN);
  }else if(strcmp(data + 7, "green")){
      leds_on(LEDS_GREEN);
      leds_off(LEDS_RED | LEDS_YELLOW);
  }


}


static void subscribes_management(void){

    int i = 0;

    for(i = 0; mqtt_state.topics[i] != NULL; i++){
	mqtt_subscribe_name(mqtt_state.topics[i]);
	mqtt_msg_subscribe(mqtt_state.connect_info, mqtt_state.data);
	uip_udp_packet_send(mqtt_state.udp_connection, mqtt_state.connect_info->buffer->data, mqtt_state.connect_info->buffer->length);
	mqtt_state.pending_msg_type = NULL;
    }

}



static void handle_mqtt_input_udp(char* data){


  switch(data[1]){


  case MQTT_MSG_TYPE_GWINFO:
    mqtt_state.connect_info->gwid = data[2];
    break;


  case MQTT_MSG_TYPE_CONNACK:
    switch(data[2]){
    case 0:
      printf("CONNACK:\t Accepted\n");
      mqtt_register();
      break;
    case 1:
      printf("CONNACK:\t Rejected: congestion\n");
      break;
    case 2:
      printf("CONNACK:\t Rejected: invalid topic ID\n");
      break;
    case 3:
      printf("CONNACK:\t Rejected: not supported\n");
      break;
    }
    break;


  case MQTT_MSG_TYPE_REGACK:
    // if MsgId is correct and return code is 0
    if( (((data[4] << 8) | data[5]) == mqtt_state.connect_info->msg_id) && data[6] == 0 ){
      mqtt_state.connect_info->topic_id = ((data[2] << 8) | data[3]);
      printf("REGACK:\t OKAY\n");
      subscribes_management();
      sensors_start = 1;
    }
    else{
      if( ((data[4] << 8) | data[5]) != mqtt_state.connect_info->msg_id ){
	  printf("REGACK:\t Wrong MsgId\n");
      }
      switch(data[6]){
      case 1:
	printf("REGACK:\t Rejected: congestion\n");
	break;
      case 2:
	printf("REGACK:\t Rejected: invalid topic ID\n");
	break;
      case 3:
	printf("REGACK:\t Rejected: not supported\n");
	break;
      }

    }
    break;


  case MQTT_MSG_TYPE_PUBLISH:
    printf("Received data: %s\n",data + 7);
    handle_publish_msg(data);
    break;


  case MQTT_MSG_TYPE_PUBACK:
    // if MsgId and topicid are correct and return code is 0
    if( 
       (((data[2] << 8) | data[3]) == mqtt_state.connect_info->topic_id) &&
       (((data[4] << 8) | data[5]) == mqtt_state.connect_info->msg_id) &&
       data[6] == 0 ){
      printf("PUBACK:\t OKAY\n");
    }
    else{
      if( ((data[2] << 8) | data[3]) != mqtt_state.connect_info->topic_id ){
	  printf("PUBACK:\t Wrong TopicId\n");
      }
      if( ((data[4] << 8) | data[5]) != mqtt_state.connect_info->msg_id ){
	  printf("PUBACK:\t Wrong MsgId\n");
      }
      switch(data[6]){
      case 1:
	printf("PUBACK:\t Rejected: congestion\n");
	break;
      case 2:
	printf("PUBACK:\t Rejected: invalid topic ID\n");
	break;
      case 3:
	printf("PUBACK:\t Rejected: not supported\n");
	break;
      }

    }
    break;


  case MQTT_MSG_TYPE_SUBACK:
    // if MsgId is correct and return code is 0
    if(	(((data[5] << 8) | data[6]) == mqtt_state.connect_info->msg_id) &&
	data[7] == 0 ){
      printf("SUBACK:\t OKAY\n");
    }
    else{
      if( ((data[5] << 8) | data[6]) != mqtt_state.connect_info->msg_id ){
	  printf("SUBACK:\t Wrong MsgId\n");
      }
      switch(data[7]){
      case 1:
	printf("SUBACK:\t Rejected: congestion\n");
	break;
      case 2:
	printf("SUBACK:\t Rejected: invalid topic ID\n");
	break;
      case 3:
	printf("SUBACK:\t Rejected: not supported\n");
	break;
      }

    }
    break;


  case MQTT_MSG_TYPE_PINGREQ:
      printf("PINGREQ:\t OKAY");
    break;


  case MQTT_MSG_TYPE_PINGRESP:
      printf("PINGRESP:\t OKAY");
    break;


  case MQTT_MSG_TYPE_UNSUBACK:
      printf("UNSUBACK:\t OKAY");
    break;


  case MQTT_MSG_TYPE_DISCONNECT:
      printf("DISCONNECT:\t OKAY");
    break;


  case MQTT_MSG_TYPE_WILLTOPICREQ:
      printf("WILLTOPICREQ:\t OKAY");
    break;


  case MQTT_MSG_TYPE_WILLMSGREQ:
      printf("WILLMSGREQ:\t OKAY");
    break;


  case MQTT_MSG_TYPE_WILLTOPICRESP:
    switch(data[2]){
    case 0:
      printf("WILLTOPICRESP:\t OKAY");
      break;
    case 1:
      printf("WILLTOPICRESP\t Rejected: congestion\n");
      break;
    case 2:
      printf("WILLTOPICRESP\t Rejected: invalid topic ID\n");
      break;
    case 3:
      printf("WILLTOPICRESP\t Rejected: not supported\n");
      break;
    }
    break;


  case MQTT_MSG_TYPE_WILLMSGRESP:
    switch(data[2]){
    case 0:
      printf("WILLMSGRESP:\t OKAY");
      break;
    case 1:
      printf("WILLMSGRESP\t Rejected: congestion\n");
      break;
    case 2:
      printf("WILLMSGRESP\t Rejected: invalid topic ID\n");
      break;
    case 3:
      printf("WILLMSGRESP\t Rejected: not supported\n");
      break;
    }
    break;

  }


}



void handle_mqtt_output_udp(void){


  if(mqtt_state.pending_msg_type != NULL){

    switch(mqtt_state.pending_msg_type){

    case MQTT_MSG_TYPE_ADVERTISE:
      mqtt_msg_advertise(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_SEARCHGW:
      mqtt_msg_searchgw(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_WILLTOPIC:
      mqtt_msg_willtopic(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_WILLMSG:
      mqtt_msg_willmsg(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_REGISTER:
      mqtt_msg_register(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_PUBLISH:
      mqtt_msg_publish(mqtt_state.connect_info, mqtt_state.data);
      break;

    case MQTT_MSG_TYPE_SUBSCRIBE:
      mqtt_msg_subscribe(mqtt_state.connect_info, mqtt_state.data);
      break;

    case MQTT_MSG_TYPE_UNSUBSCRIBE:
      mqtt_msg_unsubscribe(mqtt_state.connect_info, mqtt_state.data);
      break;

    case MQTT_MSG_TYPE_PINGREQ:
      mqtt_msg_pingreq(mqtt_state.connect_info, mqtt_state.data);
      break;

    case MQTT_MSG_TYPE_PINGRESP:
      mqtt_msg_pingresp(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_DISCONNECT:
      mqtt_msg_disconnect(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_WILLTOPICUPD:
      mqtt_msg_willtopicupd(mqtt_state.connect_info);
      break;

    case MQTT_MSG_TYPE_WILLMSGUPD:
      mqtt_msg_willmsgupd(mqtt_state.connect_info);
      break;

    }

    uip_udp_packet_send(mqtt_state.udp_connection, mqtt_state.connect_info->buffer->data, mqtt_state.connect_info->buffer->length);

    mqtt_state.pending_msg_type = NULL;

  }


}



void tcpip_handler(void)
{
  char *str;

  if(uip_newdata()) {
    str = uip_appdata;
    str[uip_datalen()] = '\0';

    handle_mqtt_input_udp(str);

  }

}

