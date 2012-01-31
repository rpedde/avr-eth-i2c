#include <inttypes.h>
#include <string.h>

#include "main.h"
#include "rnet.h"
#include "eth.h"
#include "ip.h"
#include "icmp.h"

int icmp_process_packet(uint8_t *buffer, uint16_t len) {
    if((ETH_HDR(buffer)->eth_type == htons(ETH_TYPE_IP)) &&
       (IP_HDR(buffer)->ip_p == IP_PROTO_ICMP) &&
       (ICMP_HDR(buffer)->icmp_type == ICMP_ECHO) &&
       (memcmp(&IP_HDR(buffer)->ip_dst, &myip, sizeof(ip_addr_t)) == 0)) {
        /* we have an echo request */
        xprintf("ICMP echo request");

        eth_reply_hdr(buffer);
        ip_reply_hdr(buffer);

        ICMP_HDR(buffer)->icmp_type = ICMP_ECHOREPLY;
        ICMP_HDR(buffer)->icmp_cksum = htons(ntohs(ICMP_HDR(buffer)->icmp_cksum + 8));

        enc28j60PacketSend(len, buffer);
        return 1;
    }

    return 0;
}
