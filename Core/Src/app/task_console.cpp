#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_console.h"

#include "sys_io.h"

#define TAG "TaskConsole"

TaskHandle_t CreateTaskConsole;

static CMDLine CMDLINE;
static QueueHandle_t rxCsQueue = NULL;
static uint8_t u8RxCs;

static void onEventHandle(UEvent_t *e);
static void ConsoleTaskInput(void*);

void taskConsoleHandle(void *params) {
	waitAllTasksStarted();

	APP_LOG(TAG, "Started");

	rxCsQueue = xQueueCreate(16, sizeof(uint8_t));
	ASSERT_PARAM(rxCsQueue != NULL);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	HAL_UART_Receive_IT(&huart1, &u8RxCs, sizeof(uint8_t));

	ASSERT_PARAM(pdFAIL != xTaskCreate(ConsoleTaskInput, "InputConsole", 256, NULL, 5, NULL));
	TaskPostPureMessage(TASK_CONSOLE_ID, SIG_CONSOLE_CLI_INIT);

	for (;;) {
        UEvent_t e;
		while (taskDequeueMessage(TASK_CONSOLE_ID, &e)) {
			onEventHandle(&e);
		}
	}
}

void onEventHandle(UEvent_t *e) {
    switch (e->signal) {
	case SIG_CONSOLE_CLI_INIT: {
		APP_DBG_SIG(TAG, "SIG_CONSOLE_CLI_INIT");
		
		CMDLINE.InitCMDLine((Args_t*)lgnCmdTable);
	}
	break;

	case SIG_CONSOLE_HANDLE_CMD_LINE: {
	#if 0
		APP_DBG_SIG(TAG, "SIG_CONSOLE_HANDLE_CMD_LINE");
	#endif
		switch (CMDLINE.parseLocal(' ')) {
		case CMD_SUCCESS:
		break;
		case CMD_NOT_FOUND:
		break;
		case CMD_TOO_LONG:
		break;
		case CMD_TBL_NOT_FOUND:
		break;
		default:
		break;
		}
		APP_PRINT("-| ");
	}
	break;

	default:
	break;
	}
}

void ConsoleTaskInput(void*) {
	char u8Data;

	for (;;) {
        if (pdFAIL != xQueueReceive(rxCsQueue, &u8Data, portMAX_DELAY)) {
			APP_PRINT("%c", u8Data);

			if (u8Data == '\r' || u8Data == '\n') {
				APP_PRINT("\r\n");
				TaskPostPureMessage(TASK_CONSOLE_ID, SIG_CONSOLE_HANDLE_CMD_LINE);
			}
			else if (u8Data == 8 || u8Data == 127) { /* Backspace & Delete */
				CMDLINE.removeData();
			}
			else {
				CMDLINE.addData(u8Data);
			}
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (huart->Instance == USART1) {
		if (!xQueueIsQueueFullFromISR(rxCsQueue)) {
			xQueueSendFromISR(rxCsQueue, &u8RxCs, &xHigherPriorityTaskWoken);
			if(xHigherPriorityTaskWoken) {
				portYIELD();
			}
		}
		HAL_UART_Receive_IT(&huart1, &u8RxCs, sizeof(uint8_t));
	}
}
