/*
 * Based on dhcp.h from avrlib, by Pascal Stang.  Original comments follow:
 *
 * File Name    : 'dhcp.h'
 * Title                : DHCP Protocol Library
 * Author               : Pascal Stang
 * Created              : 9/17/2005
 * Revised              : 9/17/2005
 * Version              : 0.1
 * Target MCU   : Atmel AVR series
 * Editor Tabs  : 4
 *
 *      \ingroup network
 *      \defgroup dhcp DHCP Protocol Library (dhcp.c)
 *      \code #include "net/dhcp.h" \endcode
 *      \par Description
 *              This library provides a limited implementation of DHCP (Dynamic Host
 *              Configuration Protocol) as described in RFC2131.  DHCP allows a
 *              network device to automatically obtain an IP address and other network
 *              configuration settings from a DHCP server.
 *
 *      \note This code is currently below version 1.0, and therefore is considered
 *      to be lacking in some functionality or documentation, or may not be fully
 *      tested.  Nonetheless, you can expect most functions to work.
 *
 *      This code is distributed under the GNU Public License
 *              which can be found at http://www.gnu.org/licenses/gpl.txt
 */

#ifndef _DHCP_H_
#define _DHCP_H_

typedef struct bootp_t {
    uint8_t op;             ///< Message op-code / message type
    uint8_t htype;          ///< Hardware address type   (Ethernet=1)
    uint8_t hlen;           ///< Hardware address length (Ethernet=6 byte MAC addr)
    uint8_t hops;           ///< hop count (client set to zero)
    uint32_t xid;           ///< Transaction ID (randomly chosen by client, must remain same)
    uint16_t secs;          ///< Seconds elapsed since DHCP negotiation began (filled by client)
    uint16_t flags;         ///< Flags
    uint32_t ciaddr;        ///< Client IP address (filled only if already bound, renewing, or rebinding)
    uint32_t yiaddr;        ///< 'Your' IP address (client)
    uint32_t siaddr;        ///< Server IP address
    uint32_t giaddr;        ///< Gateway IP address
    uint8_t chaddr[16];     ///< Client Hardware Address
    uint8_t sname[64];      ///< Server Host Name
    uint8_t file[128];      ///< Boot file name (null-term string)
} bootp_t;

#define BOOTP_HEADER_LEN       236   ///< length of BOOTP header not including options

#define BOOTP_OP_BOOTREQUEST     1   ///< BOOTP Request operation (message from client to server)
#define BOOTP_OP_BOOTREPLY       2   ///< BOOTP Reply operation (message from server to client)

#define BOOTP_HTYPE_ETHERNET     1
#define BOOTP_HLEN_ETHERNET      6

typedef struct dhcp_t {
    bootp_t bootp;
    uint32_t cookie;
    uint8_t options[];
} dhcp_t;

#define DHCP_HEADER_LEN        240

#define DHCP_UDP_SERVER_PORT    67   // target port for dhcp requests
#define DHCP_UDP_CLIENT_PORT    68   // target port for dhcp replies

#define DHCP_OPT_PAD             0   ///< token padding value (make be skipped)
#define DHCP_OPT_NETMASK         1   ///< subnet mask client should use (4 byte mask)
#define DHCP_OPT_ROUTERS         3   ///< routers client should use (IP addr list)
#define DHCP_OPT_TIMESERVERS     4   ///< time servers client should use (IP addr list)
#define DHCP_OPT_NAMESERVERS     5   ///< name servers client should use (IP addr list)
#define DHCP_OPT_DNSSERVERS      6   ///< DNS servers client should use (IP addr list)
#define DHCP_OPT_HOSTNAME       12   ///< host name client should use (string)
#define DHCP_OPT_DOMAINNAME     15   ///< domain name client should use (string)
#define DHCP_OPT_REQUESTEDIP    50   ///< IP address requested by client (IP address)
#define DHCP_OPT_LEASETIME      51   ///< DHCP Lease Time (uint32 seconds)
#define DHCP_OPT_DHCPMSGTYPE    53   ///< DHCP message type (1 byte)
#define DHCP_OPT_SERVERID       54   ///< Server Identifier (IP address)
#define DHCP_OPT_PARAMREQLIST   55   ///< Paramerter Request List (n OPT codes)
#define DHCP_OPT_RENEWALTIME    58   ///< DHCP Lease Renewal Time (uint32 seconds)
#define DHCP_OPT_REBINDTIME     59   ///< DHCP Lease Rebinding Time (uint32 seconds)
#define DHCP_OPT_END           255   ///< token end value (marks end of options list)

#define DHCP_MSG_DHCPDISCOVER    1   ///< DISCOVER is broadcast by client to solicit OFFER from any/all DHCP servers.
#define DHCP_MSG_DHCPOFFER       2   ///< OFFER(s) are made to client by server to offer IP address and config info.
#define DHCP_MSG_DHCPREQUEST     3   ///< REQUEST is made my client in response to best/favorite OFFER message.
#define DHCP_MSG_DHCPDECLINE     4   ///< DECLINE may be sent by client to server to indicate IP already in use.
#define DHCP_MSG_DHCPACK         5   ///< ACK is sent to client by server in confirmation of REQUEST, contains config and IP.
#define DHCP_MSG_DHCPNAK         6   ///< NAK is sent to client by server to indicate problem with REQUEST.
#define DHCP_MSG_DHCPRELEASE     7   ///< RELEASE is sent by client to server to relinquish DHCP lease on IP address, etc.
#define DHCP_MSG_DHCPINFORM      8   ///< INFORM is sent by client to server to request config info, IP address configured locally.

extern void dhcp_init(void);
extern void dhcp_tick_seconds(void);
extern int dhcp_process_packet(uint8_t *buffer, uint16_t len);

#endif /* _DHCP_H_ */
