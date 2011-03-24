/*
 *
 */
#include <inttypes.h>
#include <string.h>

#include "rnet.h"
#include "main.h"
#include "eth.h"
#include "ip.h"


/* this is some stack bloat... */
void ip_pack_hdr(uint8_t *packet, uint8_t tos, uint16_t len, uint16_t id,
                 uint16_t off, uint8_t ttl, uint8_t p, ip_addr_t src,
                 ip_addr_t dst) {
    IP_HDR(packet)->ip_v = 4;
    IP_HDR(packet)->ip_hl = 4;
    IP_HDR(packet)->ip_tos = tos;
    IP_HDR(packet)->ip_len = htons(len);
    IP_HDR(packet)->ip_id = htons(id);
    IP_HDR(packet)->ip_off = htons(off);
    IP_HDR(packet)->ip_ttl = ttl;
    IP_HDR(packet)->ip_p = p;
    IP_HDR(packet)->ip_src = src;
    IP_HDR(packet)->ip_dst = dst;
}

void ip_reply_hdr(uint8_t *packet) {
    memcpy(&IP_HDR(packet)->ip_dst, &IP_HDR(packet)->ip_src, sizeof(ip_addr_t));
    memcpy(&IP_HDR(packet)->ip_src, &myip, sizeof(ip_addr_t));
}


