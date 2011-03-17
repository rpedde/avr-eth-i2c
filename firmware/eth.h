#ifndef _ETH_H_
#define _ETH_H_

#define ETH_ADDR_LEN  6
#define ETH_HDR_LEN  14

typedef struct eth_addr_t {
    uint8_t data[ETH_ADDR_LEN];
} eth_addr_t;

/* don't really care beyond these */
#define ETH_TYPE_IP     0x0800
#define ETH_TYPE_ARP    0x0806
#define ETH_TYPE_RARP   0x8035

#define ETH_ADDR_BCAST  "\xff\xff\xff\xff\xff\xff"

typedef struct eth_header_t {
    eth_addr_t dst_addr;
    eth_addr_t src_addr;
    uint16_t eth_type;
} eth_header_t;

#define eth_pack_hdr(h, dst, src, type) do {                    \
        eth_header_t *eth_pack_p = (eth_header_t *)(h);         \
        memmove(&eth_pack_p->dst_addr, &(dst), ETH_ADDR_LEN);   \
        memmove(&eth_pack_p->src_addr, &(src), ETH_ADDR_LEN);   \
        eth_pack_p->eth_type = htons(type);                     \
    } while (0)


#endif /* _ETH_H_ */
