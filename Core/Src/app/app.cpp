#include "app.h"
#include "app_data.h"
#include "task_list.h"

void appInit() {
	InitURTOS(taskList, EOT_TASK_ID);

	Internet.Init();
	
}
 
