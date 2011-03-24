#include <inttypes.h>
#include <string.h>
#include "main.h"
#include "eth.h"
#include "rnet.h"

void eth_pack_hdr(uint8_t *packet, eth_addr_t *dst, eth_addr_t *src, uint16_t type) {
    memcpy(ETH_HDR(packet)->dst_addr.data, dst->data, ETH_ADDR_LEN);
    memcpy(ETH_HDR(packet)->src_addr.data, src->data, ETH_ADDR_LEN);
    ETH_HDR(packet)->eth_type = htons(type);
}

void eth_reply_hdr(uint8_t *packet) {
    memcpy(ETH_HDR(packet)->dst_addr.data, ETH_HDR(packet)->src_addr.data, ETH_ADDR_LEN);
    memcpy(ETH_HDR(packet)->src_addr.data, &mymac, ETH_ADDR_LEN);
}

