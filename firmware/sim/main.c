#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/* #include "ip_arp_udp.h" */
#include "enc28j60.h"
#include "timeout.h"
#include "avr_compat.h"
#include "net.h"
#include "dhcp.h"
#include "icmp.h"
#include "main.h"

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24};
uint8_t myip[4] = {10,253,1,100};
uint8_t buf[BUFFER_SIZE+1];

int main(void){
    uint16_t plen;
    uint8_t i=0;

    dhcp_init();

    while(1) {
        // get the next new packet:
        if((plen = enc28j60PacketReceive(BUFFER_SIZE, buf))) {
            if(dhcp_process_packet(buf, plen))
                continue;

            if(arp_process_packet(buf, plen))
                continue;

            if(icmp_process_packet(buf, plen))
                continue;
        } else {
            dhcp_tick_seconds();
        }
    }
    return (0);
}
