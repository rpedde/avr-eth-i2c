
#include <inttypes.h>
#include <string.h>

#include "main.h"
#include "net.h"

#include "rnet.h"
#include "eth.h"
#include "ip.h"
#include "udp.h"
#include "dhcp.h"

/* dhcp replies we are currently waiting on */
#define DHCP_STATE_DISCOVER        1 /* sent discover, waiting for offer */
#define DHCP_STATE_ACK             2 /* got offer... accepted... waiting for ack */
#define DHCP_STATE_RENEW           3 /* sent renew, waiting for ack */
#define DHCP_STATE_ACQUIRED        4 /* steady state */
#define DHCP_STATE_EXPIRED         5 /* gave up */

static uint16_t dhcp_sec_to_expire;
static uint16_t dhcp_sec_to_refresh;
static uint8_t dhcp_state;
static uint8_t dhcp_error_count;
static ip_addr_t dhcp_server_ip;
static ip_addr_t dhcp_offer_ip;

/* Forwards */
uint8_t *dhcp_set_option(uint8_t* options, uint8_t optcode,
                         uint8_t optlen, void* optvalptr);

void dhcp_send_packet(uint8_t msg_type);
void dhcp_get_lease(void);

/*
 * Initialize the dhcp subsystem
 */
void dhcp_init(void) {
    /* we don't have a lease yet */
    memset(myip, 0, sizeof(myip));
    dhcp_error_count = 0;
    dhcp_get_lease();
}

/*
 * This potentially processes a packet as a DHCP packet.
 */
int dhcp_process_packet(uint8_t *buffer, uint16_t len) {
    dhcp_t *header;

    if((((eth_header_t*)buffer)->eth_type == htons(ETH_TYPE_IP)) &&
       (((ip_header_t*)&buffer[ETH_HEADER_LEN])->ip_p == IP_PROTO_UDP) &&
       (((udp_header_t*)&buffer[ETH_HEADER_LEN + IP_HEADER_LEN])->uh_dport == htons(DHCP_UDP_CLIENT_PORT)))  {

        /* looks like it might be a dhcp packet for us! */
        header = (dhcp_t*)&buffer[ETH_HEADER_LEN + IP_HEADER_LEN +
                                  UDP_HEADER_LEN];
        if(header->bootp.xid == *(uint32_t*)&mymac) {
            /* yup, this was my transaction */
            if((dhcp_state == DHCP_STATE_DISCOVER || dhcp_state == DHCP_STATE_RENEW) &&
               header->bootp.op == BOOTP_OP_BOOTREPLY) {
                memcpy(&dhcp_offer_ip, &header->bootp.yiaddr, sizeof(dhcp_offer_ip));
                dprintf("Got DHCPOFFER");
                memcpy(&dhcp_server_ip, &header->bootp.siaddr, sizeof(dhcp_server_ip));
                dprintf("Sending DHCPREQUEST (lease)");
                dhcp_send_packet(DHCP_MSG_DHCPREQUEST);

                dhcp_state = DHCP_STATE_ACK;
                dhcp_sec_to_refresh = 10;
            } else if (dhcp_state == DHCP_STATE_ACK) {
                dhcp_state = DHCP_STATE_ACQUIRED;
                memcpy(&myip, &dhcp_offer_ip, sizeof(dhcp_offer_ip));
                dprintf("Got DHCPACK, IP initialized with %d.%d.%d.%d",
                        myip[0], myip[1], myip[2], myip[3]);
                /* need to pull out the lease duration */

                dhcp_sec_to_expire = 1200;
                dhcp_sec_to_refresh = dhcp_sec_to_expire >> 1;
            }
        }
    }

    return 0;
}

/*
 * Renew the DHCP lease.  Send request packet,
 * and wait for ACK or NAK
 */
void dhcp_renew_lease(void) {
    dprintf("Sending DHCP REQUEST (renew)");
    dhcp_state = DHCP_STATE_RENEW;
    dhcp_send_packet(DHCP_MSG_DHCPREQUEST);
    dhcp_sec_to_refresh = 10;
}

/*
 * get a full DHCP lease (from cold boot)
 */
void dhcp_get_lease(void) {
    dprintf("Sending DHCP DISCOVER");
    dhcp_send_packet(DHCP_MSG_DHCPDISCOVER);
    dhcp_state = DHCP_STATE_DISCOVER;
    dhcp_sec_to_refresh = 10;
    dhcp_sec_to_expire = 10;
}

/*
 * Get a full DHCP lease.  Send request packet,
 * then wait for an offer
 */
void dhcp_send_packet(uint8_t msg_type) {
    dhcp_t *packet;
    uint8_t *end_of_packet;
    uint32_t val;

    packet = (struct dhcp_t*)&buf[ETH_HEADER_LEN +
                                  IP_HEADER_LEN +
                                  UDP_HEADER_LEN];

    eth_pack_hdr(buf, ETH_ADDR_BCAST, (eth_addr_t*)&mymac, ETH_TYPE_IP);

    /* now need an IP header */
    ip_pack_hdr(buf, IP_TOS_DEFAULT, 0, 0, IP_DF,
                IP_TTL_DEFAULT, IP_PROTO_UDP, IP_ADDR_ANY,
                IP_ADDR_BROADCAST);

    /* now need a UDP header */
    udp_pack_hdr(&buf[ETH_HEADER_LEN + IP_HEADER_LEN],
                 DHCP_UDP_CLIENT_PORT,
                 DHCP_UDP_SERVER_PORT,
                 0);

    packet->bootp.op = BOOTP_OP_BOOTREQUEST;
    packet->bootp.htype = BOOTP_HTYPE_ETHERNET;
    packet->bootp.hlen = BOOTP_HLEN_ETHERNET;


    if(dhcp_state == DHCP_STATE_RENEW) {
        memcpy(&packet->bootp.ciaddr, myip, sizeof(myip));
    } else {
        packet->bootp.ciaddr = htonl(0);
    }
    packet->bootp.yiaddr = htonl(0);
    packet->bootp.siaddr = htonl(0);
    packet->bootp.giaddr = htonl(0);

    memcpy(packet->bootp.chaddr, mymac, 6);
    packet->bootp.xid = *(uint32_t*)&mymac;
    packet->bootp.flags = htons(1);
    packet->cookie = 0x63538263;

    val = msg_type;
    end_of_packet = dhcp_set_option(packet->options, DHCP_OPT_DHCPMSGTYPE, 1, &val);

    if(dhcp_state == DHCP_STATE_DISCOVER || dhcp_state == DHCP_STATE_RENEW) {  /* we are REQUESTING after an OFFER */
        end_of_packet = dhcp_set_option(end_of_packet, DHCP_OPT_REQUESTEDIP, 4, &dhcp_offer_ip);
        end_of_packet = dhcp_set_option(end_of_packet, DHCP_OPT_SERVERID, 4, &dhcp_server_ip);
    }

    end_of_packet = dhcp_set_option(end_of_packet, DHCP_OPT_END, 0, NULL);
    udp_send_packet(buf, end_of_packet - (uint8_t *)packet - 1);
}

/*
 * tick off the dhcp wait timer
 */
void dhcp_tick_seconds(void) {
    if(!dhcp_sec_to_refresh)
        return;

    if(!dhcp_sec_to_expire) {
        /* our lease is up... force a full recycle */
        dhcp_init();
        return;
    }

    dhcp_sec_to_refresh--;

    if(!dhcp_sec_to_refresh) {
        switch(dhcp_state) {
        case DHCP_STATE_ACQUIRED:
        case DHCP_STATE_RENEW:
            dhcp_error_count++;
            dhcp_renew_lease();
            break;
        case DHCP_STATE_DISCOVER:
            dhcp_error_count++;
            dhcp_get_lease();
            break;
        default:
            break;
        }
    }
}

uint8_t *dhcp_set_option(uint8_t* options, uint8_t optcode,
                         uint8_t optlen, void* optvalptr) {
    // set optcode
    *options++ = optcode;
    // set optlen
    *options++ = optlen;
    // copy in argument/data
    while(optlen--) {
        *options++ = *(uint8_t*)optvalptr++;
    }
    // write end marker
    *options = DHCP_OPT_END;

    // return address of end marker, to be used as a future write point
    return options;
}
