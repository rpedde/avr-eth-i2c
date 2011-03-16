#include "main.h"
#include "net.h"

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
    uint32_t val;
    uint8_t i=0;

    packet = (struct dhcp_t*)buf[ETH_HEADER_LEN +
                                 IP_HEADER_LEN +
                                 UDP_HEADER_LEN];

    make_eth(&buf, "\xff\xff\xff\xff\xff\xff")

    packet->boot.op = BOOTP_OP_BOOTREQUEST;
    packet->bootp.htype = BOOTP_HTYPE_ETHERNET;
    packet->bootp.hlen = BOOTP_HLEN_ETHERNET;

    packet->bootp.ciaddr = htonl(myip);
    packet->bootp.yiaddr = htonl(0);
    packet->bootp.siaddr = htonl(0);
    packet->bootp.giaddr = htonl(0);
    while(i < 6) {
        packet->bootp.chaddr[i] = mymac[i];
        i++;
    }
    packet->bootp.xid = (uint32_t*)&mymac;
    packet->bootp.flags = htons(1);
    packet->cookie = 0x63438263;

    val = DHCP_MSG_DHCPDISCOVER;
    dhcp_set_option(packet->options, DHCP_OPT_DHCPMSGTYPE, 1, &val);


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
