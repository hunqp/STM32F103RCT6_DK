#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdint.h>

#define VIETNAM_TIMEZONE    ( 38 )
#define HONGKONG_TIMEZONE   ( 39 )

#define NETWORK_RET_SUCCESS ( 1 )
#define NETWORK_RET_FAILURE ( -1 )

typedef struct {
    uint16_t yy;
    uint8_t mo, dd;
    uint8_t hh, mm, ss;
} NetworkDateTime_t;

extern void networkProtocolsInit();
extern int8_t dnsNetworkRun(const char *domain, uint8_t ip[]);
extern int8_t sntpNetworkRun(NetworkDateTime_t *netTime);

#endif