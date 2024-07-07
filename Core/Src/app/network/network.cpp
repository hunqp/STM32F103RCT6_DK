#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "network.h"

#include "dns.h"
#include "sntp.h"

static bool isDNSInited = false;
static bool isSNTPInited = false;

static uint8_t sntpBuffers[512];
static uint8_t dnsBuffers[512];

static void dnsBegin() {
    if (!Internet.isConnected()) {
        return;
    }
    
    DNS_init(DNS_SOCKET_NUMBER, dnsBuffers);
    isDNSInited = true;
}

static void sntpBegin() {
    if (!Internet.isConnected()) {
        return;
    }

    uint8_t ip[4] = { 0, 0, 0, 0 };

	dnsNetworkRun("time.google.com", ip);
	SNTP_init(SNTP_SOCKET_NUMBER, ip, VIETNAM_TIMEZONE, sntpBuffers); /* 38 for VN Timezone and 39 for HK Timezone */
    isSNTPInited = true;
}

void networkProtocolsInit() {
    dnsBegin();
    sntpBegin();
}

int8_t dnsNetworkRun(const char *domain, uint8_t ip[]) {
    int8_t dnsCode;
	uint8_t retryCounters = 3;
    uint8_t dns[4];

    if (!Internet.isConnected() || !isDNSInited) {
        return NETWORK_RET_FAILURE;
    }

	if (domain == NULL || strlen(domain) == 0) {
		return NETWORK_RET_FAILURE;
	}

    Internet.getDNS(dns);
    APP_DBG("DNS %d.%d.%d.%d", "Finding target IP from %s", dns[0], dns[1], dns[2], dns[3], domain);

    do {
        dnsCode = DNS_run(dns, (uint8_t*)domain, ip);
        if (dnsCode == 0) {
            if (retryCounters-- > 0) {
                break;
            }
        }
    } while (dnsCode == 0);

    if (dnsCode) {
        APP_DBG("DNS %d.%d.%d.%d", "%s Got IP %d.%d.%d.%d", dns[0], dns[1], dns[2], dns[3], domain, ip[0], ip[1], ip[2], ip[3]);
    }
    else {
        APP_DBG("DNS %d.%d.%d.%d", "Couldn't find IP from %s", dns[0], dns[1], dns[2], dns[3], domain);
    }

    return (dnsCode == 1) ? NETWORK_RET_SUCCESS : NETWORK_RET_FAILURE;
}

int8_t sntpNetworkRun(NetworkDateTime_t *netTime) {
    if (!Internet.isConnected() || !isSNTPInited) {
        return NETWORK_RET_FAILURE;
    }

    datetime dt;

    if (SNTP_run(&dt) == 1) {
        netTime->yy = dt.yy;
        netTime->mo = dt.mo;
        netTime->dd = dt.dd;
        netTime->hh = dt.hh;
        netTime->mm = dt.mm;
        netTime->ss = dt.ss;
        
        return NETWORK_RET_SUCCESS;
    }

	return NETWORK_RET_FAILURE;
}
