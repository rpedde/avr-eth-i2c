#include <inttypes.h>

#include "eth.h"
#include "ip.h"
#include "icmp.h"

int icmp_process_packet(uint8_t *buffer, uint16_t len) {
    if((ETH_HDR(buffer)->eth_type == htons(ETH_TYPE_IP)) &&
       (IP_HDR(buffer)->ip_p == IP_PROTO_ICMP) &&
       (ICMP_HDR(buffer)->icmp_type == ICMP_ECHO)) {
        /* we have an echo request */
        dprintf("ICMP echo request");
    }
}
