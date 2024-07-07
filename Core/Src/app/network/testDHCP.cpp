#include "testDHCP.h"

#define NETIF_DEFAULT_MAC			{0x00   , 0x08  , 0xdc  , 0x78, 0x91    , 0x71  }
#define NETIF_DEFAULT_IP 			{192    , 168   , 0     , 110                   }
#define NETIF_DEFAULT_SN 			{255    , 255   , 255   , 0                     }
#define NETIF_DEFAULT_GW 			{192    , 168   , 0     , 1                     }
#define NETIF_DEFAULT_DNS 			{8      , 8     , 8     , 8                     }

static wiz_NetInfo gWIZNETINFO;
static uint8_t ethBuffers[1024];

static void ipAssigned() {
	const uint8_t defaultMAC[6] = NETIF_DEFAULT_MAC;
	const uint8_t defaultSN[4]  = NETIF_DEFAULT_SN;

	printf("> DHCP : IP Assigned\r\n");
	getIPfromDHCP(gWIZNETINFO.ip);
	getGWfromDHCP(gWIZNETINFO.gw);
	getSNfromDHCP(gWIZNETINFO.sn);
	getDNSfromDHCP(gWIZNETINFO.dns);
	memcpy(gWIZNETINFO.mac, defaultMAC, 6);
	memcpy(gWIZNETINFO.sn, defaultSN, 4);
	gWIZNETINFO.dhcp = NETINFO_DHCP;
	wizchip_setnetinfo(&gWIZNETINFO);
}

static void ipUpdated() {
	printf("> DHCP : IP Updated\r\n");
}

static void ipConflicted() {
	printf("> DHCP : IP Conflicted\r\n");
}

void testDHCP() {
    const uint8_t defaultMAC[6] = NETIF_DEFAULT_MAC;
    const uint8_t defaultIP[4]  = NETIF_DEFAULT_IP;
    const uint8_t defaultSN[4]  = NETIF_DEFAULT_SN;
    const uint8_t defaultGW[4]  = NETIF_DEFAULT_GW;
    const uint8_t defaultDNS[4] = NETIF_DEFAULT_DNS;

    intr_kind intrKind = IK_DEST_UNREACH;

    const unsigned char W5x00AddressSet[2][8] = {
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

    W5x00_Reset();

    reg_wizchip_cs_cbfunc(W5x00_SetCSLow, W5x00_SetCSHigh);
    reg_wizchip_spi_cbfunc(W5x00_ReadByte, W5x00_SendByte);
    reg_wizchip_spiburst_cbfunc(W5x00_ReadData, W5x00_SendData);

    if (ctlwizchip(CW_INIT_WIZCHIP,(void*)W5x00AddressSet) != -1) {
        printf("> WIZCHIP : Init SUCCESS\r\n");
    }

    if (ctlwizchip(CW_SET_INTRMASK, &intrKind) == -1) {
		
	}

    gWIZNETINFO.dhcp = NETINFO_DHCP;
    memcpy(gWIZNETINFO.mac, defaultMAC, 6);
    memcpy(gWIZNETINFO.ip,  defaultIP,  4);
    memcpy(gWIZNETINFO.sn,  defaultSN,  4);
    memcpy(gWIZNETINFO.gw,  defaultGW,  4);
    memcpy(gWIZNETINFO.dns, defaultDNS, 4);
    wizchip_setnetinfo(&gWIZNETINFO);

    DHCP_stop();
    DHCP_init(4, ethBuffers);
    reg_dhcp_cbfunc(ipAssigned, ipUpdated, ipConflicted);

    printf("> DHCP : Running DHCP Service\r\n");
    wizchip_setnetinfo(&gWIZNETINFO);
	
    bool internetConnection = false;
    uint16_t TIMEOUT = 0;
    uint16_t TICK1S = 0;

    while (!internetConnection) {
        int rc = DHCP_run();
        if (rc == DHCP_IP_LEASED) {
            internetConnection = true;
            break;
        }

        delayInMilliseconds(1);
        ++TICK1S;

        if (TICK1S == 1000) {
            TICK1S = 0;
            DHCP_time_handler();
            rstHardWatchdog();
        }

        if (TIMEOUT++ >= 15000) {
            printf("> DHCP : Failed to acquire IP from DHCP\r\n");
            break;
        }
    }

    printf("\r\n-- INTERNET INFORMATION --\r\n");
    printf("MAC Address : %02x:%02x:%02x:%02x:%02x:%02x\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
    printf("IP  Address : %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
    printf("Subnet Mask : %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
    printf("Gateway     : %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
    printf("DNS Server  : %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
    printf("\r\n");

    while (1) {
        delayInMilliseconds(1000);
        rstHardWatchdog();
        blinkLedLife();
    }
}