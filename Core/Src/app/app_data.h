#ifndef __APP_DATA_H
#define __APP_DATA_H

#include "Internet.h"
#include "BaseMQTT.h"
#include "network.h"

typedef struct {
    struct {
        uint8_t mac[6], ip[4], sn[4], gw[4], dns[4];
    } dhcp;

    struct {
        char host[32];
        uint8_t dnsHostIP[4];
        char username[16];
        char password[16];
        uint16_t port;
    } mqtt;

    struct {
        uint16_t yy;
        uint8_t mo, dd;
        uint8_t hh, mm, ss;
    } sntp;
} NetworkInformation_t;

class MuxData {
public:
    MuxData();

    void updateDataNetwork();

    NetworkInformation_t getNetworkInformation();
private:
    NetworkInformation_t netif;
};

extern InternetHelper Internet;
extern BaseMQTT baseMQTT;

extern const char *Logo;

#endif /* __APP_DATA_H */
