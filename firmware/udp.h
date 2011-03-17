#ifndef _UDP_H_
#define _UDP_H_

#define UDP_HEADER_LEN     8

typedef struct udp_header_t {
    uint16_t uh_sport;       /* source port */
    uint16_t uh_dport;       /* destination port */
    uint16_t uh_ulen;        /* udp length (including header) */
    uint16_t uh_sum;         /* udp checksum */
} udp_header_t;

#define UDP_PORT_MAX    65535

#define udp_pack_hdr(hdr, sport, dport, ulen) do {              \
        udp_header *udp_pack_p = (udp_headder *)(hdr);          \
        udp_pack_p->uh_sport = htons(sport);                    \
        udp_pack_p->uh_dport = htons(dport);                    \
        udp_pack_p->uh_ulen = htons(ulen);                      \
    } while (0)

#endif /* _UDP_H_ */
