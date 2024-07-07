#ifndef __INTERNET_H
#define __INTERNET_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"

#define INTERNET_MUTEX_EN   ( 0 )

#define NETIF_DEFAULT_MAC	{0x00   , 0x08  , 0xdc  , 0x78  , 0x91  , 0x71}
#define NETIF_DEFAULT_IP 	{192    , 168   , 0     , 255                 }
#define NETIF_DEFAULT_SN 	{255    , 255   , 255   , 0                   }
#define NETIF_DEFAULT_GW 	{192    , 168   , 0     , 1                   }
#define NETIF_DEFAULT_DNS 	{8      , 8     , 8     , 8                   }

class InternetHelper {
public:
    enum eEthernetState {
        DownLink,
        UpLink,
    };

    InternetHelper();
    ~InternetHelper();

    void Init();
    void dhcpRestart();
    void dhcpRun();
    void tickPer1Secs();
    eEthernetState getEthernetState();
    bool isConnected();
    void getIP(uint8_t ip[]);
    void getSubnetMask(uint8_t sn[]);
    void getGateway(uint8_t gw[]);
    void getDNS(uint8_t dns[]);
    void getMAC(uint8_t mac[]);
    void dumpNetworkInformation();

    static void onConnected();
    static void onDisconnected();

private:
    const uint8_t defaultMAC[6] = NETIF_DEFAULT_MAC;
    const uint8_t defaultIP[4]  = NETIF_DEFAULT_IP;
    const uint8_t defaultSN[4]  = NETIF_DEFAULT_SN;
    const uint8_t defaultGW[4]  = NETIF_DEFAULT_GW;
    const uint8_t defaultDNS[4] = NETIF_DEFAULT_DNS;
    uint8_t ethBuffers[1024];
    bool mConnected = false;
    eEthernetState mEthState = eEthernetState::DownLink;
    
    #if INTERNET_MUTEX_EN
    SemaphoreHandle_t mMutex;
    #endif
};

#endif /* __INTERNET_H */
