/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"
#include "net/rpl/rpl.h"
#include "dev/watchdog.h"
#include "dev/slip.h"
#include "dev/leds.h"
#include "dev/cc2530-rf.h"
#include "debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint16_t len;

#define SERVER_REPLY          1

/* Should we act as RPL root? */
#define SERVER_RPL_ROOT       1

#if SERVER_RPL_ROOT
static uip_ipaddr_t ipaddr;
#endif


static uint8_t prefix_set;


#if DEBUG
#define PUTSTRING(...) putstring(__VA_ARGS__)
#define PUTHEX(...) puthex(__VA_ARGS__)
#define PUTBIN(...) putbin(__VA_ARGS__)
#define PUTDEC(...) putdec(__VA_ARGS__)
#define PUTCHAR(...) putchar(__VA_ARGS__)
#else
#define PUTSTRING(...)
#define PUTHEX(...)
#define PUTBIN(...)
#define PUTDEC(...)
#define PUTCHAR(...)
#endif

/*---------------------------------------------------------------------------*/
PROCESS(border_router_process, "Border Router process");
PROCESS_NAME(mqtt_client_process);
AUTOSTART_PROCESSES(&border_router_process, &mqtt_client_process);
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void) CC_NON_BANKED
{
  int i;
  uint8_t state;

  PUTSTRING("Router's IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state
        == ADDR_PREFERRED)) {
      PUTSTRING("  ");
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PUTCHAR('\n');
      if(state == ADDR_TENTATIVE) {
        uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
request_prefix(void) CC_NON_BANKED
{
  /* mess up uip_buf with a dirty request... */
  uip_buf[0] = '?';
  uip_buf[1] = 'P';
  uip_len = 2;
  slip_send();
  uip_len = 0;
}

/*---------------------------------------------------------------------------*/
/* Set our prefix when we receive one over SLIP */
void
set_prefix_64(uip_ipaddr_t *prefix_64)
{
  rpl_dag_t *dag;
  uip_ipaddr_t ipaddr;
  memcpy(&ipaddr, prefix_64, 16);
  prefix_set = 1;
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  /* Become root of a new DODAG with ID our global v6 address */
  dag = rpl_set_root(RPL_DEFAULT_INSTANCE, &ipaddr);
  if(dag != NULL) {
    rpl_set_prefix(dag, &ipaddr, 64);
    PUTSTRING("Created a new RPL dag with ID: ");
    PRINT6ADDR(&dag->dag_id);
    PUTCHAR('\n');
  }
}


static void
tcpip_handler(void)
{
  memset(buf, 0, MAX_PAYLOAD_LEN);
  if(uip_newdata()) {
    leds_on(LEDS_RED);
    len = uip_datalen();
    memcpy(buf, uip_appdata, len);
    PRINTF("%u bytes from [", len);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
#if SERVER_REPLY
    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
    server_conn->rport = UIP_UDP_BUF->srcport;

    uip_udp_packet_send(server_conn, buf, len);
    /* Restore server connection to allow data from any node */
    uip_create_unspecified(&server_conn->ripaddr);
    server_conn->rport = 0;
#endif
  }
  leds_off(LEDS_RED);
  return;
}




/*---------------------------------------------------------------------------*/
PROCESS_THREAD(border_router_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();
  PUTSTRING("Border Router started\n");
  prefix_set = 0;

  leds_on(LEDS_GREEN);

  /* Request prefix until it has been received */
  while(!prefix_set) {
    leds_on(LEDS_RED);
    PUTSTRING("Prefix request.\n");
    etimer_set(&et, CLOCK_SECOND);
    request_prefix();
    leds_off(LEDS_RED);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  /* We have created a new DODAG when we reach here */
  PUTSTRING("On Channel ");
  PUTDEC(cc2530_rf_channel_get());
  PUTCHAR('\n');

  print_local_addresses();

  leds_off(LEDS_GREEN);

  PUTSTRING("\nConfiguration DONE !!!!\n");

  server_conn = udp_new(NULL, UIP_HTONS(0), NULL);
  udp_bind(server_conn, UIP_HTONS(3000));

  //PUTSTRING("Listen port: 3000, TTL=%u\n", server_conn->ttl);


  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
  }


  PROCESS_EXIT();

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
