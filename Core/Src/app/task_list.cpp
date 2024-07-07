#include "task_list.h"

#include "task_system.h"
#include "task_console.h"
#include "task_cloud.h"
#include "task_dmn.h"

UTask_t taskList[] = {
    /*------------------------/
    / TASK SYSTEM
    /
    /-------------------------*/
#if TASK_SYSTEM_EN
    {
        TASK_SYSTEM_ID,
        taskSystemHandle,
        256,
        TASK_RANK_7,
        "TaskSystem",
        CreateTaskSystem,
        EVENT_MSG_NULL
    },
#endif
    /*------------------------/
    / TASK CONSOLE
    /
    /-------------------------*/
#if TASK_CONSOLE_EN
	{	
        TASK_CONSOLE_ID,
        taskConsoleHandle,
        1024,
        TASK_RANK_1,
        "TaskConsole",
        CreateTaskConsole,
        EVENT_MSG_NULL 
    },
#endif
    /*------------------------/
    / TASK CLOUD
    /
    /-------------------------*/
#if TASK_CLOUD_EN
	{	
        TASK_CLOUD_ID,
        taskCloudHandle,
        2048,
        TASK_RANK_5,
        "TaskCloud ",
        CreateTaskCloud,
        EVENT_MSG_NULL
    },
#endif

    /*------------------------/
    / TASK DEVICE MANAGER
    /
    /-------------------------*/
#if TASK_DMN_EN
	{	
        TASK_DMN_ID,
        taskDeviceManagerHandle,
        1024,
        TASK_RANK_4,
        "TaskDMn   ",
        CreateTaskDeviceManager,
        EVENT_MSG_NULL
    }
#endif
};
