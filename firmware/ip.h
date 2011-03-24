#ifndef _IP_H_
#define _IP_H_

#define IP_ADDR_LEN 4

#define IP_HEADER_LEN   20
#define IP_OPT_LEN       2
#define IP_OPT_LEN_MAX  40
#define IP_HDR_LEN_MAX  (IP_HEADER_LEN + IP_OPT_LEN_MAX)

#define IP_LEN_MAX   65535
#define IP_LEN_MIN   IP_HEADER_LEN

typedef uint32_t ip_addr_t;

typedef struct ip_header_t {
    uint8_t ip_hl:4,   /* header length (including options) */
            ip_v:4;    /* IP version */
    uint8_t ip_tos;    /* type of service */
    uint16_t ip_len;   /* total length (incl header) */
    uint16_t ip_id;    /* identification */
    uint16_t ip_off;   /* fragment offset and flags */
    uint8_t ip_ttl;    /* time to live */
    uint8_t ip_p;      /* protocol */
    uint16_t ip_sum;   /* checksum */
    ip_addr_t ip_src;  /* source address */
    ip_addr_t ip_dst;  /* destination address */
} ip_header_t;

/*
 * Type of service (ip_tos), RFC 1349 ("obsoleted by RFC 2474")
 */
#define IP_TOS_DEFAULT          0x00    /* default */
#define IP_TOS_LOWDELAY         0x10    /* low delay */
#define IP_TOS_THROUGHPUT       0x08    /* high throughput */
#define IP_TOS_RELIABILITY      0x04    /* high reliability */
#define IP_TOS_LOWCOST          0x02    /* low monetary cost - XXX */
#define IP_TOS_ECT              0x02    /* ECN-capable transport */
#define IP_TOS_CE               0x01    /* congestion experienced */

/*
 * IP precedence (high 3 bits of ip_tos), hopefully unused
 */
#define IP_TOS_PREC_ROUTINE             0x00
#define IP_TOS_PREC_PRIORITY            0x20
#define IP_TOS_PREC_IMMEDIATE           0x40
#define IP_TOS_PREC_FLASH               0x60
#define IP_TOS_PREC_FLASHOVERRIDE       0x80
#define IP_TOS_PREC_CRITIC_ECP          0xa0
#define IP_TOS_PREC_INTERNETCONTROL     0xc0
#define IP_TOS_PREC_NETCONTROL          0xe0

/*
 * Fragmentation flags (ip_off)
 */
#define IP_RF           0x8000          /* reserved */
#define IP_DF           0x4000          /* don't fragment */
#define IP_MF           0x2000          /* more fragments (not last frag) */
#define IP_OFFMASK      0x1fff          /* mask for fragment offset */

/*
 * Time-to-live (ip_ttl), seconds
 */
#define IP_TTL_DEFAULT  64              /* default ttl, RFC 1122, RFC 1340 */
#define IP_TTL_MAX      255             /* maximum ttl */

/*
 * Protocol (ip_p) - http://www.iana.org/assignments/protocol-numbers
 */

/* Don't care about the rest */
#define IP_PROTO_IP             0               /* dummy for IP */
#define IP_PROTO_ICMP           1               /* ICMP */
#define IP_PROTO_TCP            6               /* TCP */
#define IP_PROTO_UDP            17              /* UDP */


#define IP_ADDR_ANY             (htonl(0x00000000))     /* 0.0.0.0 */
#define IP_ADDR_BROADCAST       (htonl(0xffffffff))     /* 255.255.255.255 */
#define IP_ADDR_LOOPBACK        (htonl(0x7f000001))     /* 127.0.0.1 */
#define IP_ADDR_MCAST_ALL       (htonl(0xe0000001))     /* 224.0.0.1 */
#define IP_ADDR_MCAST_LOCAL     (htonl(0xe00000ff))     /* 224.0.0.255 */

#define IP_HDR(packet) ((ip_header_t *)(((uint8_t*)(packet))            \
                                        + ETH_HEADER_LEN + IP_HEADER_LEN))

/* /\* this shouldn't be a macro *\/ */
/* #define ip_pack_hdr(hdr, tos, len, id, off, ttl, p, src, dst) do {      \ */
/*         ip_header_t *ip_pack_p = (ip_header_t *)(hdr);                  \ */
/*         ip_pack_p->ip_v = 4; ip_pack_p->ip_hl = 5;                      \ */
/*         ip_pack_p->ip_tos = tos; ip_pack_p->ip_len = htons(len);        \ */
/*         ip_pack_p->ip_id = htons(id); ip_pack_p->ip_off = htons(off);   \ */
/*         ip_pack_p->ip_ttl = ttl; ip_pack_p->ip_p = p;                   \ */
/*         ip_pack_p->ip_src = src; ip_pack_p->ip_dst = dst;               \ */
/*         ip_pack_p->ip_sum = 0;                                          \ */
/*     } while (0) */

extern void ip_pack_hdr(uint8_t *packet, uint8_t tos, uint16_t len, uint16_t id,
                        uint16_t off, uint8_t ttl, uint8_t p, ip_addr_t *src,
                        ip_addr_t *dst);
extern void ip_reply_hdr(uint8_t *packet);
#endif /* _IP_H_ */
