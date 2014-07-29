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

#include <string.h>
#include <stdio.h>
#include "mqtt-msg.h"





void mqtt_msg_advertise(mqtt_connect_info_t* info)
{

  memset(info->buffer, 0, sizeof(info->buffer));

  info->buffer->data[0] = 0x05;
  info->buffer->data[1] = MQTT_MSG_TYPE_ADVERTISE;
  info->buffer->data[2] = info->gwid;
  info->buffer->data[3] = ((info->duration) & 0xff00) >> 8;
  info->buffer->data[4] = (info->duration) & 0x00ff;
  info->buffer->length = 0x05;

}



void mqtt_msg_searchgw(mqtt_connect_info_t* info)
{

  memset(info->buffer, 0, sizeof(info->buffer));

  info->buffer->data[0] = 0x03;
  info->buffer->data[1] = MQTT_MSG_TYPE_SEARCHGW;
  info->buffer->data[2] = info->radius;
  info->buffer->length = 0x03;

}



void mqtt_msg_connect(mqtt_connect_info_t* info)
{
  uint8_t header_size = 6;
  
  memset(info->buffer->data, 0, sizeof(info->buffer->data));

  if( strlen(info->client_id) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(info->client_id) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(info->client_id) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_CONNECT;
    info->buffer->data[4] = info->flags;
    info->buffer->data[5] = info->protocol_id;
    info->buffer->data[6] = ((info->duration) & 0xff00) >> 8;
    info->buffer->data[7] = (info->duration) & 0x00ff;
    memcpy(info->buffer->data + header_size, info->client_id, strlen(info->client_id));
    info->buffer->length = (strlen(info->client_id) + header_size);
  }else{
    info->buffer->data[0] = (strlen(info->client_id) + header_size);
    info->buffer->data[1] = MQTT_MSG_TYPE_CONNECT;
    info->buffer->data[2] = info->flags;
    info->buffer->data[3] = info->protocol_id;
    info->buffer->data[4] = ((info->duration) & 0xff00) >> 8;
    info->buffer->data[5] = (info->duration) & 0x00ff;
    memcpy(info->buffer->data + header_size, info->client_id, strlen(info->client_id));
    info->buffer->length = (strlen(info->client_id) + header_size);
  }

}




void mqtt_msg_willtopic(mqtt_connect_info_t* info)
{

  uint8_t header_size = 3;

  memset(info->buffer, 0, sizeof(info->buffer));


  if( strlen(info->will_topic) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(info->will_topic) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(info->will_topic) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_WILLTOPIC;
    info->buffer->data[4] = info->flags;
    memcpy(info->buffer->data + header_size, info->will_topic, strlen(info->will_topic));
    info->buffer->length = (strlen(info->will_topic) + header_size);
  }else{
    info->buffer->data[0] = (strlen(info->will_topic) + header_size);
    info->buffer->data[1] = MQTT_MSG_TYPE_WILLTOPIC;
    info->buffer->data[2] = info->flags;
    memcpy(info->buffer->data + header_size, info->will_topic, strlen(info->will_topic));
    info->buffer->length = (strlen(info->will_topic) + header_size);
  }


}



void mqtt_msg_willmsg(mqtt_connect_info_t* info)
{

  uint8_t header_size = 2;

  memset(info->buffer, 0, sizeof(info->buffer));

  if( strlen(info->will_message) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(info->will_message) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(info->will_message) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_WILLMSG;
    memcpy(info->buffer->data + header_size, info->will_message, strlen(info->will_message));
    info->buffer->length = (strlen(info->will_message) + header_size);
  }else{
    info->buffer->data[0] = (strlen(info->will_message) + header_size);
    info->buffer->data[1] = MQTT_MSG_TYPE_WILLMSG;
    memcpy(info->buffer->data + header_size, info->will_message, strlen(info->will_message));
    info->buffer->length = (strlen(info->will_message) + header_size);
  }


}




void mqtt_msg_register(mqtt_connect_info_t* info)
{

  uint8_t header_size = 6;

  memset(info->buffer, 0, sizeof(info->buffer));

  if( strlen(info->will_message) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(info->topic_name) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(info->topic_name) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_REGISTER;
    info->buffer->data[4] = (info->topic_id & 0xff00) >> 8;
    info->buffer->data[5] = info->topic_id & 0x00ff;
    info->buffer->data[6] = (info->msg_id & 0xff00) >> 8;
    info->buffer->data[7] = info->msg_id & 0x00ff;
    memcpy(info->buffer->data + header_size, info->topic_name, strlen(info->topic_name));
    info->buffer->length = (strlen(info->topic_name) + header_size);
  }else{
    info->buffer->data[0] = strlen(info->topic_name) + header_size;
    info->buffer->data[1] = MQTT_MSG_TYPE_REGISTER;
    info->buffer->data[2] = (info->topic_id & 0xff00) >> 8;
    info->buffer->data[3] = info->topic_id & 0x00ff;
    info->buffer->data[4] = (info->msg_id & 0xff00) >> 8;
    info->buffer->data[5] = info->msg_id & 0x00ff;
    memcpy(info->buffer->data + header_size, info->topic_name, strlen(info->topic_name));
    info->buffer->length = (strlen(info->topic_name) + header_size);
  }



}



void mqtt_msg_regack(mqtt_connect_info_t* info)
{

  info->buffer->data[0] = 0x07;
  info->buffer->data[1] = MQTT_MSG_TYPE_REGACK;
  info->buffer->data[2] = (info->topic_id & 0xff00) >> 8;
  info->buffer->data[3] = info->topic_id & 0x00ff;
  info->buffer->data[4] = (info->msg_id & 0xff00) >> 8;
  info->buffer->data[5] = info->msg_id & 0x00ff;
  info->buffer->data[5] = info->return_code;
  info->buffer->length = 0x07;

}




void mqtt_msg_publish(mqtt_connect_info_t* info, char* data)
{

  uint8_t header_size = 7;
  int i;

  for (i = 0; i < 5; i++)
    printf("0x%02X ", data[i]);
  printf("\n");


  printf("strlen: %d\n",strlen(data));

  if( strlen(data) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(data) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(data) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_PUBLISH;
    info->buffer->data[4] = info->flags;
    info->buffer->data[5] = (info->topic_id & 0xff00) >> 8;
    info->buffer->data[6] = info->topic_id & 0x00ff;
    info->buffer->data[7] = (info->msg_id & 0xff00) >> 8;
    info->buffer->data[8] = info->msg_id & 0x00ff;
    memcpy(info->buffer->data + header_size, data, strlen(data));
    info->buffer->length = (strlen(data) + header_size);
  }else{
    info->buffer->data[0] = strlen(data) + header_size;
    info->buffer->data[1] = MQTT_MSG_TYPE_PUBLISH;
    info->buffer->data[2] = info->flags;
    info->buffer->data[3] = (info->topic_id & 0xff00) >> 8;
    info->buffer->data[4] = info->topic_id & 0x00ff;
    info->buffer->data[5] = (info->msg_id & 0xff00) >> 8;
    info->buffer->data[6] = info->msg_id & 0x00ff;
    memcpy(info->buffer->data + header_size, data, strlen(data));
    info->buffer->length = (strlen(data) + header_size);
  }


}




void mqtt_msg_puback(mqtt_connect_info_t* info)
{

  info->buffer->data[0] = 0x07;
  info->buffer->data[1] = MQTT_MSG_TYPE_PUBACK;
  info->buffer->data[2] = (info->topic_id & 0xff00) >> 8;
  info->buffer->data[3] = info->topic_id & 0x00ff;
  info->buffer->data[4] = (info->msg_id & 0xff00) >> 8;
  info->buffer->data[5] = info->msg_id & 0x00ff;
  info->buffer->data[5] = info->return_code;
  info->buffer->length = 0x07;

}



void mqtt_msg_pubrec(mqtt_connect_info_t* info)
{

  info->buffer->data[0] = 0x04;
  info->buffer->data[1] = MQTT_MSG_TYPE_PUBREC;
  info->buffer->data[2] = (info->msg_id & 0xff00) >> 8;
  info->buffer->data[3] = info->msg_id & 0x00ff;
  info->buffer->length = 0x04;

}

void mqtt_msg_pubrel(mqtt_connect_info_t* info)
{

  info->buffer->data[0] = 0x04;
  info->buffer->data[1] = MQTT_MSG_TYPE_PUBREL;
  info->buffer->data[2] = (info->msg_id & 0xff00) >> 8;
  info->buffer->data[3] = info->msg_id & 0x00ff;
  info->buffer->length = 0x04;

}


void mqtt_msg_pubcomp(mqtt_connect_info_t* info)
{

  info->buffer->data[0] = 0x04;
  info->buffer->data[1] = MQTT_MSG_TYPE_PUBCOMP;
  info->buffer->data[2] = (info->msg_id & 0xff00) >> 8;
  info->buffer->data[3] = info->msg_id & 0x00ff;
  info->buffer->length = 0x04;

}



void mqtt_msg_subscribe(mqtt_connect_info_t* info, char* topic)
{

  uint8_t header_size;


  if( strcmp(topic,"id") == 0 ){

    header_size = 7; 
    info->buffer->data[0] = header_size;
    info->buffer->data[1] = MQTT_MSG_TYPE_SUBSCRIBE;
    info->buffer->data[2] = info->flags;
    info->buffer->data[3] = (info->msg_id & 0xff00) >> 8;
    info->buffer->data[4] = info->msg_id & 0x00ff;
    info->buffer->data[5] = (info->topic_id & 0xff00) >> 8;
    info->buffer->data[6] = info->topic_id & 0x00ff;
    info->buffer->length = header_size;

  }else if ( strcmp(topic,"name") == 0 ){

    header_size = 5; 

    if( strlen(info->topic_name) > (255-header_size) ){
      header_size = header_size + 2; 
      info->buffer->data[0] = 0x01;
      info->buffer->data[1] = ((strlen(info->topic_name) + header_size) & 0xff00) >> 8;
      info->buffer->data[2] = (strlen(info->topic_name) + header_size) & 0x00ff;
      info->buffer->data[3] = MQTT_MSG_TYPE_SUBSCRIBE;
      info->buffer->data[4] = info->flags;
      info->buffer->data[7] = (info->msg_id & 0xff00) >> 8;
      info->buffer->data[8] = info->msg_id & 0x00ff;
      memcpy(info->buffer->data + header_size, info->topic_name, strlen(info->topic_name));
      info->buffer->length = (strlen(info->topic_name) + header_size);
    }else{
      info->buffer->data[0] = strlen(info->topic_name) + header_size;
      info->buffer->data[1] = MQTT_MSG_TYPE_SUBSCRIBE;
      info->buffer->data[2] = info->flags;
      info->buffer->data[3] = (info->msg_id & 0xff00) >> 8;
      info->buffer->data[4] = info->msg_id & 0x00ff;
      memcpy(info->buffer->data + header_size, info->topic_name, strlen(info->topic_name));
      info->buffer->length = (strlen(info->topic_name) + header_size);
    }


  }


}




void mqtt_msg_unsubscribe(mqtt_connect_info_t* info, char* topic)
{

  uint8_t header_size;


  if( strcmp(topic,"id") == 0 ){

    header_size = 7; 
    info->buffer->data[0] = header_size;
    info->buffer->data[1] = MQTT_MSG_TYPE_UNSUBSCRIBE;
    info->buffer->data[2] = info->flags;
    info->buffer->data[3] = (info->msg_id & 0xff00) >> 8;
    info->buffer->data[4] = info->msg_id & 0x00ff;
    info->buffer->data[5] = (info->topic_id & 0xff00) >> 8;
    info->buffer->data[6] = info->topic_id & 0x00ff;
    info->buffer->length = header_size;

  }else if ( strcmp(topic,"name") == 0 ){

    header_size = 5; 

    if( strlen(info->topic_name) > (255-header_size) ){
      header_size = header_size + 2; 
      info->buffer->data[0] = 0x01;
      info->buffer->data[1] = ((strlen(info->topic_name) + header_size) & 0xff00) >> 8;
      info->buffer->data[2] = (strlen(info->topic_name) + header_size) & 0x00ff;
      info->buffer->data[3] = MQTT_MSG_TYPE_UNSUBSCRIBE;
      info->buffer->data[4] = info->flags;
      info->buffer->data[7] = (info->msg_id & 0xff00) >> 8;
      info->buffer->data[8] = info->msg_id & 0x00ff;
      memcpy(info->buffer->data + header_size, info->topic_name, strlen(info->topic_name));
      info->buffer->length = (strlen(info->topic_name) + header_size);
    }else{
      info->buffer->data[0] = strlen(info->topic_name) + header_size;
      info->buffer->data[1] = MQTT_MSG_TYPE_UNSUBSCRIBE;
      info->buffer->data[2] = info->flags;
      info->buffer->data[3] = (info->msg_id & 0xff00) >> 8;
      info->buffer->data[4] = info->msg_id & 0x00ff;
      memcpy(info->buffer->data + header_size, info->topic_name, strlen(info->topic_name));
      info->buffer->length = (strlen(info->topic_name) + header_size);
    }


  }


}





void mqtt_msg_pingreq(mqtt_connect_info_t* info, const char* dest)
{

  uint8_t header_size = 2;


  if( strlen(dest) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(dest) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(dest) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_PINGREQ;
    memcpy(info->buffer->data + header_size, dest, strlen(dest));
    info->buffer->length = (strlen(dest) + header_size);
  }else{
    info->buffer->data[0] = (strlen(info->client_id) + header_size);
    info->buffer->data[1] = MQTT_MSG_TYPE_PINGREQ;
    memcpy(info->buffer->data + header_size, dest, strlen(dest));
    info->buffer->length = (strlen(dest) + header_size);
  }


}



void mqtt_msg_pingresp(mqtt_connect_info_t* info)
{

  info->buffer->data[0] = 0x02;
  info->buffer->data[1] = MQTT_MSG_TYPE_PINGRESP;
  info->buffer->length = 0x02;

}



void mqtt_msg_disconnect(mqtt_connect_info_t* info)
{

  info->buffer->data[0] = 0x04;
  info->buffer->data[1] = MQTT_MSG_TYPE_DISCONNECT;
  info->buffer->data[2] = ((info->duration) & 0xff00) >> 8;
  info->buffer->data[3] = (info->duration) & 0x00ff;
  info->buffer->length = 0x04;

}



void mqtt_msg_willtopicupd(mqtt_connect_info_t* info)
{

  uint8_t header_size = 3;


  if( strlen(info->will_topic) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(info->will_topic) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(info->will_topic) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_WILLTOPICUPD;
    info->buffer->data[4] = info->flags;
    memcpy(info->buffer->data + header_size, info->will_topic, strlen(info->will_topic));
    info->buffer->length = (strlen(info->will_topic) + header_size);
  }else{
    info->buffer->data[0] = (strlen(info->will_topic) + header_size);
    info->buffer->data[1] = MQTT_MSG_TYPE_WILLTOPICUPD;
    info->buffer->data[2] = info->flags;
    memcpy(info->buffer->data + header_size, info->will_topic, strlen(info->will_topic));
    info->buffer->length = (strlen(info->will_topic) + header_size);
  }


}



void mqtt_msg_willmsgupd(mqtt_connect_info_t* info)
{

  uint8_t header_size = 2;

  if( strlen(info->will_message) > (255-header_size) ){
    header_size = header_size + 2; 
    info->buffer->data[0] = 0x01;
    info->buffer->data[1] = ((strlen(info->will_message) + header_size) & 0xff00) >> 8;
    info->buffer->data[2] = (strlen(info->will_message) + header_size) & 0x00ff;
    info->buffer->data[3] = MQTT_MSG_TYPE_WILLMSGUPD;
    memcpy(info->buffer->data + header_size, info->will_message, strlen(info->will_message));
    info->buffer->length = (strlen(info->will_message) + header_size);
  }else{
    info->buffer->data[1] = strlen(info->will_message) + header_size;
    info->buffer->data[3] = MQTT_MSG_TYPE_WILLMSGUPD;
    memcpy(info->buffer->data + header_size, info->will_message, strlen(info->will_message));
    info->buffer->length = (strlen(info->will_message) + header_size);
  }

}
