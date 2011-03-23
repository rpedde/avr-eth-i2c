/*
 *
 */

#ifndef _ARP_H_
#define _ARP_H_

#include "eth.h"
#include "ip.h"

#define ARP_HEADER_LEN     28

/* we'll only be answering ETH/IP types... */

typedef struct arp_header_t {
    uint16_t ar_hrd; /* format of hardware address */
    uint16_t ar_pro; /* format of protocol address */
    uint8_t ar_hln; /* length of hardware address */
    uint8_t ar_pln; /* length of protocol address */
    uint16_t ar_op; /* operation */
    uint8_t ar_sha[ETH_ADDR_LEN]; /* sender HA */
    uint8_t ar_spa[IP_ADDR_LEN];  /* sender PA */
    uint8_t ar_tha[ETH_ADDR_LEN]; /* target HA */
    uint8_t ar_tpa[IP_ADDR_LEN];  /* target PA */
} arp_header_t;

#define ARP_HDR(packet) ((arp_header_t*)((uint8_t*)(packet) + ETH_HEADER_LEN))

#define ARP_HRD_ETH 0x01

#define ARP_PRO_IP 0x0800

#define ARP_OP_REQUEST    1 /* produce HA given PA */ // ARP
#define ARP_OP_REPLY      2 /* response */

/* extern void arp_pack_hdr_ethip(arp_header_t *header, uint16_t op, uint8_t *sha, */
/*                                uint8_t *spa, uint8_t tha, uint8_t tpa); */
extern int arp_process_packet(uint8_t *buffer, uint16_t len);

#endif /* _ARP_H_ */
