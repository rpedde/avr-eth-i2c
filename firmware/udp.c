/*
 *
 */

#include <inttypes.h>

#include "main.h"
#include "ip.h"
#include "eth.h"
#include "udp.h"
#include "rnet.h"

#include "enc28j60.h"

void udp_send_packet(uint8_t *buffer, uint16_t payload_length) {
    /* set up the lengths */
    uint16_t checksum;

    UDP_HDR(buffer)->uh_sum = 0;
    IP_HDR(buffer)->ip_sum = 0;

    ((udp_header_t*)(buffer + ETH_HEADER_LEN + IP_HEADER_LEN))->uh_ulen = htons(payload_length + UDP_HEADER_LEN);
    ((ip_header_t*)(buffer + ETH_HEADER_LEN))->ip_len = htons(payload_length + UDP_HEADER_LEN + IP_HEADER_LEN);

    /* calculate UDP checksum */
    checksum = internet_checksum((uint8_t*)&((ip_header_t*)(buffer + ETH_HEADER_LEN))->ip_src,
                                 payload_length + 16,
                                 CHECKSUM_TYPE_UDP);
    ((udp_header_t*)(buffer + ETH_HEADER_LEN + IP_HEADER_LEN))->uh_sum =
        htons(checksum);

    /* do the IP checksum */
    checksum = internet_checksum((uint8_t*)IP_HDR(buffer),
                                 IP_HEADER_LEN,
                                 CHECKSUM_TYPE_IP);

    IP_HDR(buffer)->ip_sum = htons(checksum);

    enc28j60PacketSend(ETH_HEADER_LEN + IP_HEADER_LEN +
                       UDP_HEADER_LEN + payload_length,
                       buffer);
}
