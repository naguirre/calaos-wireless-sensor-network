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
#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H


#include "mqtt-msg.h"
#include "process.h"
#include "contiki-net.h"


extern int sensors_start;


void mqtt_connect(uip_ip6addr_t* address, uint16_t port, 
		  int auto_reconnect, mqtt_connect_info_t* info, const char* topics[]);

void mqtt_advertise(void);

void mqtt_searchgw(void);

void mqtt_willtopic(char* willtopic);

void mqtt_willmsg(char* willmsg);

void mqtt_register(void);

void mqtt_publish(char* data);

void mqtt_subscribe_name(char* topic);

void mqtt_subscribe_id(uint16_t id);

void mqtt_unsubscribe_name(char* topic);

void mqtt_unsubscribe_id(uint16_t id);

void mqtt_pingreq(char* dest);

void mqtt_pingresp(void);

void mqtt_disconnect(void);

void mqtt_willtopicupd(char* willtopic);

void mqtt_willmsgupd(char* willmsg);

void tcpip_handler(void);

void handle_mqtt_output_udp(void);

#endif
