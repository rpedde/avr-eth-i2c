
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
#define DHCP_STATE_INIT            1
#define DHCP_STATE_OFFER           2
#define DHCP_STATE_ACK             3
#define DHCP_STATE_ACQUIRED        4

static uint8_t dhcp_min_to_refresh;
static uint8_t dhcp_sec_to_refresh;
static uint8_t dhcp_state;

/* Forwards */
uint8_t *dhcp_set_option(uint8_t* options, uint8_t optcode,
                         uint8_t optlen, void* optvalptr);


/*
 * Initialize the dhcp subsystem
 */
void dhcp_init(void) {
    dhcp_state = DHCP_STATE_INIT;
}

/*
 * Get a full DHCP lease.  Send request packet,
 * then wait for an offer
 */
void dhcp_get_lease(void) {
    dhcp_t *packet;
    uint8_t *end_of_packet;
    uint8_t val;

    packet = (struct dhcp_t*)&buf[ETH_HEADER_LEN +
                                  IP_HEADER_LEN +
                                  UDP_HEADER_LEN];

    eth_pack_hdr((eth_header_t*)&buf, ETH_ADDR_BCAST, (eth_addr_t*)&mymac, ETH_TYPE_IP);

    /* now need an IP header */
    ip_pack_hdr(&buf[ETH_HEADER_LEN], IP_TOS_DEFAULT, 0, 0, IP_DF,
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

    //    memcpy(&packet->bootp.ciaddr, &myip, 4);
    packet->bootp.ciaddr = htonl(0);
    packet->bootp.yiaddr = htonl(0);
    packet->bootp.siaddr = htonl(0);
    packet->bootp.giaddr = htonl(0);
    memcpy(packet->bootp.chaddr, mymac, 6);
    packet->bootp.xid = *(uint32_t*)&mymac;
    packet->bootp.flags = htons(1);
    packet->cookie = 0x63538263;

    val = DHCP_MSG_DHCPDISCOVER;
    end_of_packet = dhcp_set_option(packet->options, DHCP_OPT_DHCPMSGTYPE, 1, &val);
    end_of_packet = dhcp_set_option(end_of_packet, DHCP_OPT_END, 0, NULL);

    udp_send_packet(buf, end_of_packet - (uint8_t *)packet - 1);
}

/*
 * tick off the dhcp wait timer
 */
void dhcp_tick_seconds(void) {
    if(dhcp_sec_to_refresh || dhcp_min_to_refresh) {
        if(dhcp_sec_to_refresh) {
            dhcp_sec_to_refresh--;
        } else {
            dhcp_sec_to_refresh=59;
            dhcp_min_to_refresh--;
        }
    }
}

/*
 * this is valid only on startup.  During a
 * renewal, this could be false, even when we have
 * a valid DHCP address
 */
int dhcp_has_address(void) {
    return dhcp_state == DHCP_STATE_ACQUIRED;
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
