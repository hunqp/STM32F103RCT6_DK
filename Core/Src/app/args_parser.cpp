#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_console.h"

#include "cmd_line.h"

#include "sys_io.h"
#include "sys_dbg.h"

static int8_t onInformation(char*);
static int8_t onHelp(char*);
static int8_t onReset(char*);
static int8_t onFatal(char*);
static int8_t onDebug(char*);

const Args_t lgnCmdTable[] = {
	{(const char*)"info",		onInformation,	(const char*)"System information"	},
	{(const char*)"help",		onHelp,			(const char*)"Help information"	    },
	{(const char*)"rst",		onReset,		(const char*)"Reset system"		    },
	{(const char*)"fatal",		onFatal,		(const char*)"Fatal information"	},
	{(const char*)"dbg",		onDebug,		(const char*)"Debugging"			},
	{(const char*)0,			(onCMDLine)0,	(const char*)0					    }
};

int8_t onInformation(char *args) {
	(void)args;

	Internet.dumpNetworkInformation();
	baseMQTT.dumpMQTTInformation();

	return 0;
}

int8_t onHelp(char *args) {
	(void)args;

	

 	return 0;
}

int8_t onReset(char *args) {
 	(void)args;
 	APP_PRINT("[RESTART] System !!!\r\n");
 	softReset();

 	return 0;
}

int8_t onFatal(char *args) {
 	switch (*(args + 6)) {
 	case 'l': {
		
 	}
 	break;

 	case 't': {
 		FATAL("TEST");
 	}
 	break;

 	default:
 	break;
	}

 	return 0;
}

int8_t onDebug(char *args) {
 	switch (*(args + 4)) {
 	case 'a': {
		APP_PRINT("> DEBUG : Start API TaskPostDynamicMessage()\r\n");
		timerSet(TASK_SYSTEM_ID, SIG_SYSTEM_PERIOD_5_SECONDS_DO, 500, true);
 	}
 	break;

 	case 'b': {
		APP_PRINT("> DEBUG : Stop API TaskPostDynamicMessage()\r\n");
		timerRemove(TASK_SYSTEM_ID, SIG_SYSTEM_PERIOD_5_SECONDS_DO);
 	}
 	break;

 	case 'c': {
		
 	}
 	break;

 	case 'd': {

 	}
 	break;
	
 	default:
 	break;
 	}

 	return 0;
}
