#ifndef _ETH_H_
#define _ETH_H_

#define ETH_ADDR_LEN     6
#define ETH_HDR_LEN     14
#define ETH_HEADER_LEN  14

typedef struct eth_addr_t {
    uint8_t data[ETH_ADDR_LEN];
} eth_addr_t;

/* don't really care beyond these */
#define ETH_TYPE_IP     0x0800
#define ETH_TYPE_ARP    0x0806
#define ETH_TYPE_RARP   0x8035

#define ETH_ADDR_BCAST  (eth_addr_t*)"\xff\xff\xff\xff\xff\xff"

#define ETH_HDR(packet) ((eth_header_t*)((uint8_t*)(packet)))

typedef struct eth_header_t {
    eth_addr_t dst_addr;
    eth_addr_t src_addr;
    uint16_t eth_type;
} eth_header_t;

extern void eth_pack_hdr(uint8_t *packet, eth_addr_t *dst, eth_addr_t *src, uint16_t type);
extern void eth_reply_hdr(uint8_t *packet);

#endif /* _ETH_H_ */
