#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "sys/etimer.h"

#include <string.h>
#include <stdio.h>

#include "dev/leds.h"
#include "mqtt-service.h"
#include "one_wire.h"

#include "debug.h"

#define DEBUG DEBUG_NONE
#include "net/uip-debug.h"

#define MAX_KEEPALIVE_ERROR 5

typedef enum _mqtt_state_t
{
    MQTT_STATE_CONNECTION_CLOSED,
    MQTT_STATE_CONNECTION_IN_PROGRESS,
    MQTT_STATE_CONNECTED,
}mqtt_state_t;

typedef struct _topic_t {
    char name[32];
    uint16_t id;
}topic_t;

static char topic_name[32];


typedef struct mqtt_handler_t
{
    uip_ip6addr_t address;
    uint16_t port;
    struct uip_udp_conn* udp_connection;
    mqtt_state_t state;
    uint8_t data[64];
    uint8_t name[32];
    uint8_t len;
    uint8_t con_retry;
    uint16_t keepalive;
    uint8_t keepalive_error;
    struct process* calling_process;
    uint8_t pending_msg;
    topic_t topics[8];
    uint8_t nb_topics;

} mqtt_handler_t;

process_event_t mqtt_event;
static mqtt_handler_t mqtt;
static struct etimer *topiclist;
static char temperature[16];

PROCESS(mqtt_process, "MQTT Process");

static void mqtt_msg_connect_send(mqtt_handler_t *mqtt)
{
    uint8_t header_size = 6;

    memset(mqtt->data, 0, sizeof(mqtt->data));

    mqtt->data[0] = (strlen(mqtt->name) + header_size);
    mqtt->data[1] = MQTT_MSG_CONNECT;
    mqtt->data[2] = 0;
    mqtt->data[3] = 1;
    mqtt->data[4] = ((100) & 0xff00) >> 8;
    mqtt->data[5] = (100) & 0x00ff;
    memcpy(mqtt->data + header_size, mqtt->name, strlen(mqtt->name));
    mqtt->len = (strlen(mqtt->name) + header_size);
}

static void mqtt_msg_ping_send(mqtt_handler_t *mqtt, uint8_t type)
{
    mqtt->data[0] = 2;
    mqtt->data[1] = type;
}


/* Public API */
void mqtt_connect(uip_ip6addr_t* address, uint16_t port, uint16_t keepalive, char *name)
{
    if(process_is_running(&mqtt_process))
    {
        printf("process is already running\n");
        return;
    }

    /* Coppy the host address and port */
    memcpy(&mqtt.address, address, sizeof(uip_ip6addr_t));
    mqtt.port = port;
    mqtt.keepalive = keepalive * CLOCK_SECOND;
    mqtt.keepalive_error = 0;
    mqtt.calling_process = PROCESS_CURRENT();
    mqtt.nb_topics = 0;
    strcpy(mqtt.name, name);
    /* Set connection in progress */
    mqtt.state = MQTT_STATE_CONNECTION_IN_PROGRESS;
    process_start(&mqtt_process, (const char*)&mqtt);
}

uint8_t mqtt_connected(void)
{
    if (mqtt.state == MQTT_STATE_CONNECTED)
        return 1;
    else
        return 0;
}

/* Subscribe to the specified topic */
void mqtt_subscribe(const char *topic)
{
    uint8_t header_size = 5;
    int i;

    printf("Subscribe : %s\n", topic);

    if (mqtt.state != MQTT_STATE_CONNECTED)
        return;


    for (i = 0; i < mqtt.nb_topics; i++)
    {
        if (!strcmp(topic, mqtt.topics[i].name))
        {
            printf("Topic alread registered with id : %d\n", mqtt.topics[i].id);
            return;
        }
    }

    printf("Nb topics : %d\n", mqtt.nb_topics);

    mqtt.data[0] = (strlen(topic) + header_size);
    mqtt.data[1] = MQTT_MSG_SUBSCRIBE;
    mqtt.data[2] = 0;
    mqtt.data[3] = (0 & 0xff00) >> 8;
    mqtt.data[4] = (0 & 0x00ff);
    memcpy(mqtt.data + header_size, topic, strlen(topic));
    mqtt.len = (strlen(topic) + header_size);
    mqtt.pending_msg = MQTT_MSG_SUBSCRIBE;
    strcpy(mqtt.topics[mqtt.nb_topics].name, topic);
    printf("Send packet\n");
    uip_udp_packet_send(mqtt.udp_connection, mqtt.data, mqtt.len);
}

/* Subscribe to the specified topic */
void mqtt_msg_publish(const char *topic, const char *data)
{
    uint8_t header_size = 7;
    int i;
    uint8_t topic_id = 0;

    printf("Publish : %s %s\n", topic, data);

    if (mqtt.state != MQTT_STATE_CONNECTED)
        return;


    for (i = 0; i < mqtt.nb_topics; i++)
    {
        if (!strcmp(topic, mqtt.topics[i].name))
        {
            topic_id = mqtt.topics[i].id;
            printf("topic_id found : %d\n", topic_id);
        }
    }

    mqtt.data[0] = (strlen(topic) + header_size);
    mqtt.data[1] = MQTT_MSG_PUBLISH;
    mqtt.data[2] = 0;
    mqtt.data[3] = (topic_id & 0xff00) >> 8;
    mqtt.data[4] = (topic_id & 0x00ff);
    mqtt.data[5] = 0;
    mqtt.data[6] = 0;
    memcpy(mqtt.data + header_size, data, strlen(data));
    mqtt.len = (strlen(data) + header_size);
    mqtt.pending_msg = MQTT_MSG_PUBLISH;
    printf("Send packet topicid : %d\n", topic_id);
    uip_udp_packet_send(mqtt.udp_connection, mqtt.data, mqtt.len);
}


static void handle_mqtt_input(char* data)
{
    mqtt_event_data_t event_data;

    printf("New data received data [%d] [%d]\n", data[0], data[1]);

    switch(data[1])
    {
    case MQTT_MSG_CONNACK:
    {
        PRINTF("CONNACK received %d\n", data[2]);
        switch(data[2])
        {
        case 0:
            mqtt.state = MQTT_STATE_CONNECTED;
            mqtt.keepalive_error = 0;
            printf("start keepalive timer\n");

            printf("stop connection timer\n");
            //etimer_stop(&connection_timer);
            event_data.type = MQTT_EVENT_TYPE_CONNECTED;
            printf("post connect event\n");
            process_post_synch(mqtt.calling_process, mqtt_event, &event_data);

            memset(topic_name, 0, sizeof(topic_name));
            memcpy(topic_name, "temp/", 5);
            onewire_id_str_get(topic_name + 5);
            printf("Topic name : %s\n", topic_name);
            mqtt_subscribe(topic_name);

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
    }
    case MQTT_MSG_PINGREQ:
    {
        PRINTF("PINGREQ received\n");
        /* Send ping response */
        mqtt_msg_ping_send(&mqtt, MQTT_MSG_PINGRESP);
        uip_udp_packet_send(mqtt.udp_connection, mqtt.data, mqtt.len);
        /* Reset the ping timer as the conenction is alive */


        break;
    }
    case MQTT_MSG_PINGRESP:
    {
        PRINTF("PINGRESP received\n");
        /* We get an answer to pingreq, reset keepalive error counter */
        mqtt.keepalive_error = 0;
        break;
    }
    case MQTT_MSG_SUBACK:
    {
        PRINTF("SUBACK received\n");
        if (mqtt.pending_msg == MQTT_MSG_SUBSCRIBE)
        {
            mqtt.topics[mqtt.nb_topics].id = data[3] << 8 | data[4];
            printf("Topic %s subscribed with id :%d\n", mqtt.topics[mqtt.nb_topics].name, mqtt.topics[mqtt.nb_topics].id);
            mqtt.nb_topics++;
        }
        else
        {
            printf("Error : we get suback without subscribe\n");
        }
        break;
    }
    case MQTT_MSG_PUBLISH:
    {
        int i;
        uint16_t id = data[3] << 8 | data[4];
        uint8_t len = data[0] - 7;

        printf("MQTT_MSG_PUBLISH %d\n", id);
        if (mqtt.state != MQTT_STATE_CONNECTED)
            return;

        for (i = 0; i < mqtt.nb_topics; i++)
        {
            if (mqtt.topics[i].id == id)
            {

                event_data.type  = MQTT_EVENT_TYPE_PUBLISH;
                event_data.topic = mqtt.topics[i].name;
                event_data.data  = data + 7;
                mqtt.data[0] = 7;
                mqtt.data[1] = MQTT_MSG_PUBACK;
                mqtt.data[2] = data[2];
                mqtt.data[3] = data[3];
                mqtt.data[4] = data[4];
                mqtt.data[5] = data[5];
                mqtt.data[6] = 0;
                mqtt.len = 7;
                uip_udp_packet_send(mqtt.udp_connection, mqtt.data, mqtt.len);
                process_post_synch(mqtt.calling_process, mqtt_event, &event_data);
                break;
            }
        }

        break;
    }
    default:
        PRINTF("UNNOWN MESSAGE %d\n", data[1]);
    }

}

static void periodic_timer_cb(void)
{
    mqtt_event_data_t event_data;
    uint8_t val, frac;
    uint16_t temp;

    printf("Ping timer %d\n", mqtt.keepalive_error);

    if (mqtt.state != MQTT_STATE_CONNECTED)
    {
        /* Not connected : try to reconnect */
        printf("Try to reconnect\n");
        mqtt_msg_connect_send(&mqtt);
        uip_udp_packet_send(mqtt.udp_connection, mqtt.data, mqtt.len);
        return;
    }


    if (mqtt.keepalive_error >= MAX_KEEPALIVE_ERROR)
    {

        PRINTF("DISCONNECTED\n");
        mqtt.state = MQTT_STATE_CONNECTION_IN_PROGRESS;
        event_data.type = MQTT_EVENT_TYPE_DISCONNECTED;
        process_post_synch(mqtt.calling_process, mqtt_event, &event_data);
        return;
    }
    else
    {
        mqtt.keepalive_error++;
    }

    temp = onewire_temp_read();

    val = temp / 100;
    frac = temp % 100;
    sprintf(temperature, "%d.%d", val, frac);
    printf("temperature : %s\n", temperature);
    mqtt_msg_publish(topic_name, temperature);

    mqtt_msg_ping_send(&mqtt, MQTT_MSG_PINGREQ);
    uip_udp_packet_send(mqtt.udp_connection, mqtt.data, mqtt.len);
}


PROCESS_THREAD(mqtt_process, ev, data)
{
    char *str;
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    topiclist = NULL;
    onewire_init();
    while(1)
    {
        /* connect to the server */
        PRINTF("mqtt: connecting...\n");
        /* Create a new udp connection */
        mqtt.udp_connection = udp_new(&mqtt.address, mqtt.port, NULL);
        if (mqtt.udp_connection != NULL)
        {
            PRINTF("Created a connection with the server ");
            PRINT6ADDR(&mqtt.udp_connection->ripaddr);
            PRINTF(" local/remote port %u/%u\n",
                   UIP_HTONS(mqtt.udp_connection->lport), UIP_HTONS(mqtt.udp_connection->rport));
        }
        else
        {
            PRINTF("Could not open connection\n");
        }

        /* Send a connect message to the broker */
        mqtt_msg_connect_send(&mqtt);
        PRINTF("send message CONNECT\n");
        uip_udp_packet_send(mqtt.udp_connection, mqtt.data, mqtt.len);

        /* Initialize timer for keepalive */
        printf("Keepalive : %d\n", mqtt.keepalive);
        etimer_set(&periodic_timer, mqtt.keepalive);
        /* We are not yet connected so disable keepalive timer for now */
        //etimer_stop(&ping_timer);
        while(1)
        {
            PROCESS_WAIT_EVENT();
            if (ev == tcpip_event && uip_newdata())
            {
                str = uip_appdata;
                str[uip_datalen()] = '\0';
                handle_mqtt_input(str);
            }
            else if (ev == PROCESS_EVENT_TIMER && data == &periodic_timer)
            {
                printf("PERIODIC\n");
                periodic_timer_cb();
                etimer_reset(&periodic_timer);
            }
        }
    }
    PROCESS_END();
}
