#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* packets */
#define PKT_SIZE 60
#define PKT_SIZE_MAX 1024

/* node3 */
#define DST_MAC 0x0c, 0x42, 0xa1, 0x8c, 0xdb, 0xf4
/* node2 */
#define SRC_MAC 0x0c, 0x42, 0xa1, 0x8c, 0xdc, 0x0c
#define ETH_TYPE 0x08, 0x00
#define IP_HDRS 0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01, 0xaf, 0xb6
#define SRC_IP 0x0A, 0x00, 0x00, 0x05             // src ip (10.0.0.5)
#define DST_IP 0x0A, 0x00, 0x00, 0x06             // dst ip (10.0.0.6)
#define IP_OPT 0x08, 0x00, 0x59, 0xd0, 0x88
#define ICMP_HDR 0x2c, 0x00, 0x09, 0x52, 0xae, 0x96, 0x57, 0x00, 0x00
#define UDP_HDR 0x2c, 0x00, 0x09, 0x52, 0xae, 0x96, 0x57, 0x00, 0x00

extern const uint8_t pkt_template[PKT_SIZE];

// You may define more packet related functions here!

#ifdef __cplusplus
}
#endif

#endif
