#include "app.h"
#include "app_dbg.h"
#include "Internet.h"

#include "wizchip_conf.h"
#include "dhcp.h"

#include "sys_io.h"

#define TAG     "InternetHelper"

static wiz_NetInfo gWIZNETINFO;

extern "C" void ipUpdating()  {
    const uint8_t defaultMAC[6] = NETIF_DEFAULT_MAC;
    const uint8_t defaultSN[4]  = NETIF_DEFAULT_SN;

    getIPfromDHCP(gWIZNETINFO.ip);
	getGWfromDHCP(gWIZNETINFO.gw);
	getSNfromDHCP(gWIZNETINFO.sn);
	getDNSfromDHCP(gWIZNETINFO.dns);

    memcpy(gWIZNETINFO.mac, defaultMAC, 6);
	memcpy(gWIZNETINFO.sn,  defaultSN,  4);

    gWIZNETINFO.dhcp = NETINFO_DHCP;
	wizchip_setnetinfo(&gWIZNETINFO);
}

extern "C" void dhcpIPAssigned(void) {
    APP_DBG(TAG, "DHCP IP Assigned");

    ipUpdating();
}

extern "C" void dhcpIPUpdated(void) {
    APP_DBG(TAG, "DHCP IP Updated");

    ipUpdating();
}

extern "C"  void dhcpIPConflicted(void) {
	APP_DBG(TAG, "DHCP IP Conflicted");

    ipUpdating();
}

InternetHelper::InternetHelper() {
   
}

InternetHelper::~InternetHelper() {

}

void InternetHelper::Init() {
    const unsigned char W5x00AddressSet[2][8] = {
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    W5x00_Reset();

    reg_wizchip_cs_cbfunc(W5x00_SetCSLow, W5x00_SetCSHigh);
    reg_wizchip_spi_cbfunc(W5x00_ReadByte, W5x00_SendByte);
    reg_wizchip_spiburst_cbfunc(W5x00_ReadData, W5x00_SendData);

	if (ctlwizchip(CW_INIT_WIZCHIP,(void*)W5x00AddressSet) == -1) {
		APP_DBG(TAG, "[FAILED] CW_INIT_WIZCHIP");
	}

    intr_kind intrKind = IK_DEST_UNREACH;
    if (ctlwizchip(CW_SET_INTRMASK, &intrKind) == -1) {
        APP_DBG(TAG, "[FAILED] CW_SET_INTRMASK");
    }

    gWIZNETINFO.dhcp = NETINFO_DHCP;
    memcpy(gWIZNETINFO.mac, defaultMAC, 6);
	memcpy(gWIZNETINFO.ip,  defaultIP,  4);
	memcpy(gWIZNETINFO.sn,  defaultSN,  4);
	memcpy(gWIZNETINFO.gw,  defaultGW,  4);
	memcpy(gWIZNETINFO.dns, defaultDNS, 4);
    wizchip_setnetinfo(&gWIZNETINFO);

    #if INTERNET_MUTEX_EN
    mMutex = xSemaphoreCreateMutex();
    #endif
}

void InternetHelper::dhcpRestart() {
    if (mEthState == DownLink) {
        return;
    }

    #if INTERNET_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

    memset(ethBuffers, 0, sizeof(ethBuffers));
    DHCP_stop();
    DHCP_init(4, ethBuffers);
    reg_dhcp_cbfunc(dhcpIPAssigned, dhcpIPUpdated, dhcpIPConflicted);
    wizchip_setnetinfo(&gWIZNETINFO);

    #if INTERNET_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

    APP_PRINT("> DHCP : Reset DHCP Service\r\n");
}

void InternetHelper::dhcpRun() {
    if (mEthState == DownLink) {
        return;
    }

    #if INTERNET_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

    APP_PRINT("> DHCP : Run DHCP Service\r\n");
    int rc = DHCP_run();
    if (rc == DHCP_IP_LEASED) {
        mConnected = true;
    }

    if (mConnected) {
        wizchip_getnetinfo(&gWIZNETINFO);
        InternetHelper::onConnected();
    }

    #if INTERNET_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif
}

void InternetHelper::tickPer1Secs() {
    DHCP_time_handler();
}

bool InternetHelper::isConnected() {
    #if INTERNET_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

    bool bConnected = mConnected;

    #if INTERNET_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

    return bConnected;
}

InternetHelper::eEthernetState InternetHelper::getEthernetState() {
    int8_t phyLink = PHY_LINK_OFF;
    eEthernetState state;

    #if INTERNET_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

    int8_t ret = ctlwizchip(CW_GET_PHYLINK, &phyLink);
    if (ret != -1){
        state = (phyLink == PHY_LINK_ON) ? UpLink : DownLink;
    }

    if (state != mEthState) {
        mEthState = state;
        if (mEthState == DownLink) {
            mConnected = false;
            InternetHelper::onDisconnected();
        }
    }

#if 1
    APP_DBG(TAG, "[%d] Link PHY Status: %d -> %s", ret, phyLink, state == UpLink ? "Up" : "Down");
#endif

    #if INTERNET_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

    return state;
}

void InternetHelper::dumpNetworkInformation() {
    APP_PRINT("\r\n-- INTERNET INFORMATION --\r\n");
    APP_PRINT("MAC Address : %02x:%02x:%02x:%02x:%02x:%02x\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
	APP_PRINT("IP  Address : %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	APP_PRINT("Subnet Mask : %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	APP_PRINT("Gateway     : %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
	APP_PRINT("DNS Server  : %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
    APP_PRINT("\r\n");
}

void InternetHelper::getIP(uint8_t ip[]) {
    memcpy(ip, gWIZNETINFO.ip, 4);
}

void InternetHelper::getSubnetMask(uint8_t sn[]) {
    memcpy(sn, gWIZNETINFO.sn, 4);
}

void InternetHelper::getGateway(uint8_t gw[]) {
    memcpy(gw, gWIZNETINFO.gw, 4);
}

void InternetHelper::getDNS(uint8_t dns[]) {
    memcpy(dns, gWIZNETINFO.dns, 4);
}

void InternetHelper::getMAC(uint8_t mac[]) {
    memcpy(mac, gWIZNETINFO.mac, 6);
}
