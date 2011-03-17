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
