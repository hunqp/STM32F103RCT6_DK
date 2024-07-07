#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"

#include "BaseMQTT.h"
#include "MQTTDaemon.h"

#define TAG	"MQTTDaemon"

static bool isLooped = false;

void MQTTDaemon::onService(void*) {
	#define MQTT_KEEP_ALIVE_INTERVAL	( 30000 )

	uint32_t pingREQRESP;

	baseMQTT.InitConnection(DEFAULT_MQTT_BROKER_HOST, DEFAULT_MQTT_BROKER_PORT);

	while (isLooped) {
		/* MQTT hasn't Connected -> Try Connecting */
		if (baseMQTT.getState() == BaseMQTT::eState::Disconnected) {
			baseMQTT.tryConnect();
			vTaskDelay(75); /* Wait for connection establish */
		}

		if (baseMQTT.isConnected()) {
			baseMQTT.onYEILD();

			if (xTaskGetTickCount() - pingREQRESP > MQTT_KEEP_ALIVE_INTERVAL) {
				pingREQRESP = xTaskGetTickCount();
				TaskPostPureMessage(TASK_CLOUD_ID, SIG_CLOUD_MQTT_PERIODIC_KEEP_ALIVE);
			}
		}
		else vTaskDelay(5000);

		vTaskDelay(5); /* Yeild in 5 Milliseconds */
	}

	baseMQTT.forceDisconnect();

	vTaskDelete(NULL);
}

void MQTTDaemon::Start() {
    if (!isLooped) {
		isLooped = true;
        ASSERT_PARAM(pdFAIL != xTaskCreate(MQTTDaemon::onService, "MQTTService", 512, NULL, 1, NULL));
		APP_PRINT("[START] MQTT Daemon Service\r\n");
    }
}

void MQTTDaemon::Stop() {
    isLooped = false;
	APP_PRINT("[STOP] MQTT Daemon Service\r\n");
}

void BaseMQTT::onMessage(MessageData *msg) {
	char *topic = (char*)msg->topicName->lenstring.data;
	int topicLen = msg->topicName->lenstring.len;

	char *payload = (char*)msg->message->payload;
	int payloadLen = msg->message->payloadlen;

	if (strlen(topic) > 0 && strlen(payload) > 0) {
		BaseMQTTMessage_t baseMsg;

		baseMsg.topic = (char*)malloc(sizeof(char) * (topicLen + 1));
		baseMsg.payload = (char*)malloc(sizeof(char) * (payloadLen + 1));

		if (baseMsg.topic != NULL && baseMsg.payload != NULL) {
			memcpy(baseMsg.topic, topic, topicLen);
			memcpy(baseMsg.payload, payload, payloadLen);
			baseMsg.topic[topicLen] = 0;
			baseMsg.payload[payloadLen] = 0;

			APP_DBG(TAG, "Topic  : %s", baseMsg.topic);
			APP_DBG(TAG, "PayLoad: %s", baseMsg.payload);

			TaskPostDynamicMessage(TASK_CLOUD_ID, SIG_CLOUD_MQTT_ON_MESSAGE, (uint8_t*)&baseMsg, sizeof(BaseMQTTMessage_t));
		}
	}
}

void BaseMQTT::onConnected() {
	TaskPostPureMessage(TASK_CLOUD_ID, SIG_CLOUD_MQTT_ON_CONNECTED);
}

void BaseMQTT::onDisconnected() {

}
