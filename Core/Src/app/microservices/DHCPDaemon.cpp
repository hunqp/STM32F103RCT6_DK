#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"

#include "DHCPDaemon.h"

#include "sys_io.h"

#define TAG	"DHCPDaemon"

static bool isLooped = false;

void DHCPDaemon::onService(void*) {
    #define DHCP_TIMEOUT	( 10000 )
	#define DHCP_TIMEHDL	( 1000 )

	uint32_t u32DHCPTimeout;
	uint32_t u32DHCPTimeHdl;

	while (isLooped) {
		/* DownLink -> YEILD 1 Seconds waiting for port plug-in */
		if (Internet.getEthernetState() == InternetHelper::DownLink) {
			vTaskDelay(1000);
			continue;
		}

		/* UpLink & Internet connected -> YEILD 5 Seconds */
		if (Internet.isConnected()) {
			vTaskDelay(5000);
			continue;
		}

		/* UpLink & Internet disconnected -> Start DHCP */
		u32DHCPTimeout = u32DHCPTimeHdl = xTaskGetTickCount();
		Internet.dhcpRestart();

		while (xTaskGetTickCount() - u32DHCPTimeout < DHCP_TIMEOUT) {
			Internet.dhcpRun();

			if (xTaskGetTickCount() - u32DHCPTimeHdl > 1000) {
				u32DHCPTimeHdl = xTaskGetTickCount();
				Internet.tickPer1Secs();
			}

			if (Internet.isConnected()) {
				break;
			}
			
			vTaskDelay(5);
		}
	}

	vTaskDelete(NULL);
}

void DHCPDaemon::Start() {
    if (!isLooped) {
		isLooped = true;
        ASSERT_PARAM(pdFAIL != xTaskCreate(DHCPDaemon::onService, "DHCPService", 512, NULL, 2, NULL));
		APP_PRINT("[START] DHCP Daemon Service\r\n");
    }
}

void DHCPDaemon::Stop() {
    isLooped = false;
	APP_PRINT("[STOP] DHCP Daemon Service\r\n");
}

/*----------------------------------------------------------------------/
 * InternetHelper Static Methods
 *----------------------------------------------------------------------/
 */
void InternetHelper::onConnected() {
	TaskPostPureMessage(TASK_CLOUD_ID, SIG_CLOUD_INTERNET_ON_CONNECTED);
}

void InternetHelper::onDisconnected() {
	TaskPostPureMessage(TASK_CLOUD_ID, SIG_CLOUD_INTERNET_ON_DISCONNECTED);
}

/*----------------------------------------------------------------------/
 * GPIO External Interrupt Function
 *----------------------------------------------------------------------/
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (HAL_GPIO_ReadPin(W5500_INT_GPIO_Port, W5500_INT_Pin) == GPIO_PIN_RESET) {
        APP_PRINT("W5500_INT_Pin is RESET\r\n");
    }
    else {
        APP_PRINT("W5500_INT_Pin is SET\r\n");
    }
}