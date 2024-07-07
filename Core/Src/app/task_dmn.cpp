#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_dmn.h"

#include "sys_io.h"

#define TAG "TaskDeviceManager"

TaskHandle_t CreateTaskDeviceManager;

static void onEventHandle(UEvent_t *e);

void taskDeviceManagerHandle(void *params) {
	waitAllTasksStarted();

	APP_LOG(TAG, "Started");

	for (;;) {
        UEvent_t e;
		while (taskDequeueMessage(TASK_DMN_ID, &e)) {
			onEventHandle(&e);
		}
	}
}

void onEventHandle(UEvent_t *e) {
    switch (e->signal) {
	case SIG_DMN_CONTROL_CM4_POWER_REQ: {
		APP_DBG_SIG(TAG, "SIG_DMN_CONTROL_CM4_POWER_REQ");
		
		static uint32_t counters = 0;
		const char *msg = (const char*)e->message.data;

		APP_DBG(TAG, "COUNT: %ld, Len: %d, Message : %s\r\n", ++counters, strlen(msg), msg);
	}
	break;

	case SIG_DMN_CONTROL_NX_POWER_REQ: {
		APP_DBG_SIG(TAG, "SIG_DMN_CONTROL_NX_POWER_REQ");

	}
	break;

	default:
	break;
	}
}

