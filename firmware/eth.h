#ifndef _ETH_H_
#define _ETH_H_

#define ETH_ADDR_LEN     6
#define ETH_HDR_LEN     14
#define ETH_HEADER_LEN  14

typedef uint8_t* eth_addr_t;

/* don't really care beyond these */
#define ETH_TYPE_IP     0x0800
#define ETH_TYPE_ARP    0x0806
#define ETH_TYPE_RARP   0x8035

#define ETH_ADDR_BCAST  (uint8_t*)"\xff\xff\xff\xff\xff\xff"

typedef struct eth_header_t {
    eth_addr_t dst_addr;
    eth_addr_t src_addr;
    uint16_t eth_type;
} eth_header_t;

extern void eth_pack_hdr(eth_header_t *packet, eth_addr_t dst, eth_addr_t src, uint16_t type);

#endif /* _ETH_H_ */
