/*
 *
 */

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "eth.h"
#include "ip.h"
#include "arp.h"

int arp_process_packet(uint8_t *buffer, uint16_t len) {
    if(ETH_HDR(buffer)->eth_type == htons(ETH_TYPE_ARP)) {
        if((ARP_HDR(buffer)->ar_op == htons(ARP_OP_REQUEST)) &&
           (memcmp(ARP_HDR(buffer)->ar_tpa, myip, IP_ADDR_LEN) == 0)) {
            dprintf("Got arp request");
            /* we'll in-place this */
            memcpy(&ETH_HDR(buffer)->dst_addr,
                   &ETH_HDR(buffer)->src_addr,
                   sizeof(eth_addr_t));
            memcpy(&ETH_HDR(buffer)->src_addr,
                   &mymac, sizeof(eth_addr_t));

            memcpy(&ARP_HDR(buffer)->ar_tha,
                   &ARP_HDR(buffer)->ar_sha, sizeof(eth_addr_t));
            memcpy(&ARP_HDR(buffer)->ar_tpa,
                   &ARP_HDR(buffer)->ar_spa, sizeof(ip_addr_t));
            memcpy(&ARP_HDR(buffer)->ar_sha, &mymac, sizeof(eth_addr_t));
            memcpy(&ARP_HDR(buffer)->ar_spa, &myip, sizeof(ip_addr_t));

            ARP_HDR(buffer)->ar_op = htons(ARP_OP_REPLY);

            enc28j60PacketSend(ETH_HEADER_LEN + ARP_HEADER_LEN, buffer);
            return 1;
        }

    }

    return 0;
}
