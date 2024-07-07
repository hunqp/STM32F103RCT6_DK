#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_cloud.h"

#include "DHCPDaemon.h"
#include "SNTPDaemon.h"
#include "MQTTDaemon.h"

#include "sys_io.h"

#define TAG "TaskCloud"

TaskHandle_t CreateTaskCloud;

static void onEventHandle(UEvent_t *e);

void taskCloudHandle(void *params) {
	waitAllTasksStarted();

	APP_LOG(TAG, "Started");

	DHCPDaemon::Start();
	
	for (;;) {
        UEvent_t e;
		while (taskDequeueMessage(TASK_CLOUD_ID, &e)) {
			onEventHandle(&e);
		}
	}
}

void onEventHandle(UEvent_t *e) {
    switch (e->signal) {

	case SIG_CLOUD_INTERNET_ON_CONNECTED: {
		APP_DBG_SIG(TAG, "SIG_CLOUD_INTERNET_ON_CONNECTED");

		Internet.dumpNetworkInformation();
		networkProtocolsInit();
		MQTTDaemon::Start();
		SNTPDaemon::Start();
	}
	break;

	case SIG_CLOUD_INTERNET_ON_DISCONNECTED: {
		APP_DBG_SIG(TAG, "SIG_CLOUD_INTERNET_ON_DISCONNECTED");

		MQTTDaemon::Stop();
		SNTPDaemon::Stop();
	}
	break;

	case SIG_CLOUD_SNTP_ON_CONNECTED: {
		APP_DBG_SIG(TAG, "SIG_CLOUD_SNTP_ON_CONNECTED");

		NetworkDateTime_t *dt = (NetworkDateTime_t*)e->message.data;

		APP_PRINT("NTP Timezone: %02d/%02d/%d, %02d:%02d:%02d\r\n", 
						dt->dd, dt->mo, dt->yy, 
						dt->hh, dt->mm, dt->ss);
	}
	break;

	case SIG_CLOUD_MQTT_ON_CONNECTED: {
		APP_DBG_SIG(TAG, "SIG_CLOUD_MQTT_ON_CONNECTED");

		baseMQTT.dumpMQTTInformation();
		baseMQTT.performSubcribe(DEFAULT_MQTT_SUB_TOPIC, QOS0);
	}
	break;

	case SIG_CLOUD_MQTT_PERIODIC_KEEP_ALIVE: {
		APP_DBG_SIG(TAG, "SIG_CLOUD_MQTT_PERIODIC_KEEP_ALIVE");

		const char *payload = (const char*)
		"{							\
			\"temperature\":35.5,	\
			\"humidity\":67.8		\
		}";

		int rc = baseMQTT.performPublish(DEFAULT_MQTT_PUB_TOPIC, (void*)payload, strlen(payload));
		if (rc != MQTT_RET_SUCCESS) {
			baseMQTT.setState(BaseMQTT::eState::Disconnected);
		}
	}
	break;

	case SIG_CLOUD_MQTT_ON_MESSAGE: {
		APP_DBG_SIG(TAG, "SIG_CLOUD_MQTT_ON_MESSAGE");

		BaseMQTTMessage_t msg = *((BaseMQTTMessage_t*)e->message.data);
		const char *topic = (const char*)msg.topic;
		const char *payload = (const char*)msg.payload;

		APP_DBG(TAG, "Len: %d, Topic   : %s", strlen(topic), topic);
		APP_DBG(TAG, "Len: %d, Payload : %s", strlen(payload), payload);
		 
		{
			/* TODO: Handle message */
		}

		free(msg.topic);
		free(msg.payload);
	}
	break;

	default:
	break;
	}
}
