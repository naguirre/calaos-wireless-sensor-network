#ifndef _MQTT_SERVICE_H_
#define _MQTT_SERVICE_H_

enum mqtt_message_type
{
    MQTT_MSG_ADVERTISE     = 0x00,
    MQTT_MSG_SEARCHGW      = 0x01,
    MQTT_MSG_GWINFO        = 0x02,
    MQTT_MSG_CONNECT       = 0x04,
    MQTT_MSG_CONNACK       = 0x05,
    MQTT_MSG_WILLTOPICREQ  = 0x06,
    MQTT_MSG_WILLTOPIC     = 0x07,
    MQTT_MSG_WILLMSGREQ    = 0x08,
    MQTT_MSG_WILLMSG       = 0x09,
    MQTT_MSG_REGISTER      = 0x0A,
    MQTT_MSG_REGACK        = 0x0B,
    MQTT_MSG_PUBLISH       = 0x0C,
    MQTT_MSG_PUBACK        = 0x0D,
    MQTT_MSG_PUBCOMP       = 0x0E,
    MQTT_MSG_PUBREC        = 0x0F,
    MQTT_MSG_PUBREL        = 0x10,
    MQTT_MSG_SUBSCRIBE     = 0x12,
    MQTT_MSG_SUBACK        = 0x13,
    MQTT_MSG_UNSUBSCRIBE   = 0x14,
    MQTT_MSG_UNSUBACK      = 0x15,
    MQTT_MSG_PINGREQ       = 0x16,
    MQTT_MSG_PINGRESP      = 0x17,
    MQTT_MSG_DISCONNECT    = 0x18,
    MQTT_MSG_WILLTOPICUPD  = 0x1A,
    MQTT_MSG_WILLTOPICRESP = 0x1B,
    MQTT_MSG_WILLMSGUPD    = 0x1C,
    MQTT_MSG_WILLMSGRESP   = 0x1D,
};

#define MQTT_EVENT_TYPE_NONE                  0
#define MQTT_EVENT_TYPE_CONNECTED             1
#define MQTT_EVENT_TYPE_DISCONNECTED          2
#define MQTT_EVENT_TYPE_SUBSCRIBED            3
#define MQTT_EVENT_TYPE_UNSUBSCRIBED          4
#define MQTT_EVENT_TYPE_PUBLISH               5
#define MQTT_EVENT_TYPE_PUBLISHED             6
#define MQTT_EVENT_TYPE_EXITED                7
#define MQTT_EVENT_TYPE_PUBLISH_CONTINUATION  8

typedef struct mqtt_event_data_t
{
  uint8_t type;
  const char* topic;
  const char* data;
  uint16_t topic_length;
  uint16_t data_length;
  uint16_t data_offset;

} mqtt_event_data_t;

extern process_event_t mqtt_event;

void mqtt_connect(uip_ip6addr_t* address, uint16_t port, uint16_t keepalive);
uint8_t mqtt_connected(void);
void mqtt_subscribe(const char *topic);

#endif /*  _MQTT_SERVICE_H_ */
