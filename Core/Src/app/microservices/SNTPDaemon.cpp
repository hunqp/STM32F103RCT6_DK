#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"

#include "SNTPDaemon.h"

#define TAG		"SNTPDaemon"

static bool isLooped = false;

void SNTPDaemon::onService(void*) {
    #define SNTP_TIMEOUT		( 60000  )

	NetworkDateTime_t dt;
	uint32_t u32SNTPTimeout = xTaskGetTickCount();

	memset(&dt, 0, sizeof(dt));
	while (sntpNetworkRun(&dt) != NETWORK_RET_SUCCESS && isLooped) {
		if (xTaskGetTickCount() - u32SNTPTimeout > SNTP_TIMEOUT) {
			break;
		}
		vTaskDelay(100);
	}

	TaskPostDynamicMessage(TASK_CLOUD_ID, SIG_CLOUD_SNTP_ON_CONNECTED, (uint8_t*)&dt, sizeof(dt));
	vTaskDelete(NULL);
}

void SNTPDaemon::Start() {
	if (!isLooped) {
		isLooped = true;
        ASSERT_PARAM(pdFAIL != xTaskCreate(SNTPDaemon::onService, "SNTPService", 256, NULL, 1, NULL));
		APP_PRINT("[START] SNTP Daemon Service\r\n");
	}
}

void SNTPDaemon::Stop() {
    isLooped = false;
	APP_PRINT("[STOP] SNTP Daemon Service\r\n");
}
