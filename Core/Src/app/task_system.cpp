#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_system.h"

#include "sys_io.h"

#define TAG "TaskSystem"

TaskHandle_t CreateTaskSystem;

static void onEventHandle(UEvent_t *e);

void taskSystemHandle(void *params) {
	waitAllTasksStarted();

	APP_LOG(TAG, "Started");
	timerSet(TASK_SYSTEM_ID, SIG_SYSTEM_KEEP_PING_ALIVE, SIG_SYSTEM_KEEP_ALIVE_INTERVAL, true);

	for (;;) {
        UEvent_t e;
		while (taskDequeueMessage(TASK_SYSTEM_ID, &e)) {
			onEventHandle(&e);
		}
	}
}

void onEventHandle(UEvent_t *e) {
    switch (e->signal) {
	case SIG_SYSTEM_KEEP_PING_ALIVE: {
	#if 0
		APP_DBG_SIG(TAG, "SIG_SYSTEM_KEEP_PING_ALIVE");
	#endif

		blinkLedLife();
		rstHardWatchdog();
	}
	break;

	case SIG_SYSTEM_REBOOT: {
		APP_DBG_SIG(TAG, "SYSTEM_SOFT_REBOOT");
		
		softReset();
	}
	break;

	case SIG_SYSTEM_PERIOD_5_SECONDS_DO: {
		APP_DBG_SIG(TAG, "SIG_SYSTEM_PERIOD_5_SECONDS_DO");
		
		TaskPostDynamicMessage(TASK_DMN_ID,
							   SIG_DMN_CONTROL_CM4_POWER_REQ,
							   (uint8_t*)Logo, 
							   strlen(Logo));
	}
	break;

	case SIG_SYSTEM_PERIOD_10_SECONDS_DO: {
		APP_DBG_SIG(TAG, "SIG_SYSTEM_PERIOD_10_SECONDS_DO");

	}
	break;

	case SIG_SYSTEM_PERIOD_15_SECONDS_DO: {
		APP_DBG_SIG(TAG, "SIG_SYSTEM_PERIOD_15_SECONDS_DO");
	}
	break;

	default:
	break;
	}
}

