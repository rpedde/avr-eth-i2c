#include <inttypes.h>
#include <string.h>
#include "eth.h"
#include "rnet.h"

void eth_pack_hdr(eth_header_t *packet, eth_addr_t *dst, eth_addr_t *src, uint16_t type) {
    memcpy(packet->dst_addr.data, dst->data, ETH_ADDR_LEN);
    memcpy(packet->src_addr.data, src->data, ETH_ADDR_LEN);
    packet->eth_type = htons(type);
}

