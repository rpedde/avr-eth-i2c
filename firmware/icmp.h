/*
 *
 */

#ifndef _ICMP_H_
#define _ICMP_H_

typedef struct icmp_header_t {
    uint8_t icmp_type;
    uint8_t icmp_code;
    uint16_t icmp_cksum;
} icmp_header_t;

#define ICMP_CODE_NONE   0  /* for types without codes */
#define ICMP_ECHOREPLY   0  /* echo reply */
#define ICMP_ECHO        8  /* echo service */

#define ICMP_HDR(packet) ((icmp_header_t *)((uint8_t *)(packet) + \
    ETH_HEADER_LEN + IP_HEADER_LEN))

extern int icmp_process_packet(uint8_t *buffer, uint16_t len);

#endif /* _ICMP_H_ */
