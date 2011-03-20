#include <inttypes.h>
#include "rnet.h"

uint16_t htons(uint16_t s) {
    return ((s << 8) | (s >> 8));
}

uint32_t htonl(uint32_t l) {
    return ((l << 24) | ((l & 0x00FF0000L) >> 8) |
            ((l & 0x0000FF00L) << 8) |
            ( l >> 24));
}

uint16_t internet_checksum(uint8_t *buf, uint16_t len, uint8_t type) {
    uint32_t sum = 0;

    if(type == 1) {
        sum = IP_PROTO_UDP;
        sum += len - 8;
    }

    if(type == 2) {
        sum = IP_PROTO_TCP;
        sum += len - 8;
    }

    while(len> 1) {
        sum += 0xFFFF & (*buf << 8 | *(buf + 1));
        buf += 2;
        len -= 2;
    }

    if(len) {
        sum += (0xFF & *buf) << 8;
    }

    while(sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ((uint16_t) sum ^ 0xFFFF);
}
