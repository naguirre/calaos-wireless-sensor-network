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

#ifndef MQTT_MSG_H
#define MQTT_MSG_H



#include <stdint.h>

enum mqtt_message_type
{
  MQTT_MSG_TYPE_ADVERTISE     = 0x00,
  MQTT_MSG_TYPE_SEARCHGW      = 0x01,
  MQTT_MSG_TYPE_GWINFO        = 0x02,
  MQTT_MSG_TYPE_CONNECT       = 0x04,
  MQTT_MSG_TYPE_CONNACK       = 0x05,
  MQTT_MSG_TYPE_WILLTOPICREQ  = 0x06,
  MQTT_MSG_TYPE_WILLTOPIC     = 0x07,
  MQTT_MSG_TYPE_WILLMSGREQ    = 0x08,
  MQTT_MSG_TYPE_WILLMSG       = 0x09,
  MQTT_MSG_TYPE_REGISTER      = 0x0A,
  MQTT_MSG_TYPE_REGACK        = 0x0B,
  MQTT_MSG_TYPE_PUBLISH       = 0x0C,
  MQTT_MSG_TYPE_PUBACK        = 0x0D,
  MQTT_MSG_TYPE_PUBCOMP       = 0x0E,
  MQTT_MSG_TYPE_PUBREC        = 0x0F,
  MQTT_MSG_TYPE_PUBREL        = 0x10,
  MQTT_MSG_TYPE_SUBSCRIBE     = 0x12,
  MQTT_MSG_TYPE_SUBACK        = 0x13,
  MQTT_MSG_TYPE_UNSUBSCRIBE   = 0x14,
  MQTT_MSG_TYPE_UNSUBACK      = 0x15,
  MQTT_MSG_TYPE_PINGREQ       = 0x16,
  MQTT_MSG_TYPE_PINGRESP      = 0x17,
  MQTT_MSG_TYPE_DISCONNECT    = 0x18,
  MQTT_MSG_TYPE_WILLTOPICUPD  = 0x1A,
  MQTT_MSG_TYPE_WILLTOPICRESP = 0x1B,
  MQTT_MSG_TYPE_WILLMSGUPD    = 0x1C,
  MQTT_MSG_TYPE_WILLMSGRESP   = 0x1D,
};



typedef struct mqtt_message
{
  uint8_t data[64];
  uint16_t length;

} mqtt_message_t;


typedef struct mqtt_connect_info
{
  char* client_id;
  mqtt_message_t* buffer;
  uint16_t duration;
  uint8_t flags;
  uint8_t gwid;
  uint16_t msg_id;
  uint8_t protocol_id;
  uint8_t radius;
  uint8_t return_code;
  uint16_t topic_id;
  char* topic_name;
  char* will_message;
  char* will_topic;
 } mqtt_connect_info_t;


void mqtt_msg_advertise(mqtt_connect_info_t* info);
void mqtt_msg_searchgw(mqtt_connect_info_t* info);
void mqtt_msg_connect(mqtt_connect_info_t* info);
void mqtt_msg_willtopic(mqtt_connect_info_t* info);
void mqtt_msg_willmsg(mqtt_connect_info_t* info);
void mqtt_msg_register(mqtt_connect_info_t* info);
void mqtt_msg_regack(mqtt_connect_info_t* info);
void mqtt_msg_publish(mqtt_connect_info_t* info, char* data);
void mqtt_msg_puback(mqtt_connect_info_t* info);
void mqtt_msg_pubrec(mqtt_connect_info_t* info);
void mqtt_msg_pubrel(mqtt_connect_info_t* info);
void mqtt_msg_pubcomp(mqtt_connect_info_t* info);
void mqtt_msg_subscribe(mqtt_connect_info_t* info,char* topic);
void mqtt_msg_unsubscribe(mqtt_connect_info_t* info, char* topic);
void mqtt_msg_pingreq(mqtt_connect_info_t* info, char* dest);
void mqtt_msg_pingresp(mqtt_connect_info_t* info);
void mqtt_msg_disconnect(mqtt_connect_info_t* info);
void mqtt_msg_willtopicupd(mqtt_connect_info_t* info);
void mqtt_msg_willmsgupd(mqtt_connect_info_t* info);


#endif
