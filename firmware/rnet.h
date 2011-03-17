#ifndef _RNET_H_
#define _RNET_H_

#include "eth.h"
#include "ip.h"

/* #define htons(s) (uint16_t)((((uint16_t)s)<<8) | (((uint16_t)s)>>8)) */
/* #define htonl(l) ((l<<24) | ((l&0x00FF0000l)>>8) | ((l&0x0000FF00l)<<8) | (l>>24)) */
struct eth_addr_t;
struct ip_addr_t;

extern eth_addr_t rnet_hwaddr;
extern eth_addr_t rnet_gw_hwaddr;
extern ip_addr_t rnet_ipaddr;

extern uint16_t htons(uint16_t);
extern uint32_t htonl(uint32_t);

extern int rnet_init(eth_addr_t *hwaddr, ip_addr_t ipaddr, ip_addr_t netmask);
extern int rnet_set_gw(ip_addr_t ipaddr);


#endif /* _RNET_H_ */
