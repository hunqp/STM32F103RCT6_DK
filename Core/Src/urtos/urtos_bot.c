#include "urtos_bot.h"
#include "sys_io.h"

#define LOG_PRINT_EN          (1)
#define LOG_DBG_EN            (0)

#define LOCAL_PRINT         	printf

#if LOG_PRINT_EN
#define LOCAL_LOG(fmt, ...) 	LOCAL_PRINT("[ProBOT] " "\x1B[35m" fmt "\e[0m" "\r\n", ##__VA_ARGS__)
#else
#define LOCAL_LOG(fmt, ...)
#endif

#if LOG_DBG_EN
#define LOCAL_DBG(fmt, ...) 	LOCAL_PRINT("[DBG]->" "\e[0;36m" fmt "\e[0m" "\r\n", ##__VA_ARGS__)
#else
#define LOCAL_DBG(fmt, ...)
#endif

typedef struct {
	UTask_t *taskTbl;
	uint16_t taskTblSize;
} TaskManager_t;

static TaskManager_t taskManager;
static SemaphoreHandle_t semaSyncTasksStarted = NULL;

void assertParams(bool IsTrue, const char *file, uint32_t line) {
    bool firstPrint = true;
    if (!IsTrue) {
        ENTRY_CRITICAL();
        do {
            if (firstPrint) {
                firstPrint = false;
                LOCAL_PRINT("[ASSERT]\r\n");
                LOCAL_PRINT(" -File: %s\r\n", file);
                LOCAL_PRINT(" -Line: %ld\r\n", line);
            }
            blinkLedLife();
            rstHardWatchdog();
            delayInMilliseconds(50);
        } while(ASSERT);

        softReset();
    }
}

static void bannerIntro() {
    LOCAL_PRINT("\r\n"
            " __ \\                                           \r\n"
            " |   |  |   |  __ \\    _` |  __ `__ \\    _ \\  \r\n"
            " |   |  |   |  |   |  (   |  |   |   |  (   |    \r\n"
            "____/  \\__, | _|  _| \\__,_| _|  _|  _| \\___/  \r\n"
            "       ____/                                     \r\n"
            );
    LOCAL_PRINT("MicroRTOS version: %s\r\n", VERSION);
    LOCAL_PRINT(" -Bot: %s\r\n", CYBORG);
    LOCAL_PRINT(" -Run as: %s\r\n", BUILD);
    LOCAL_PRINT(" -Date: %s, %s\r\n", __DATE__, __TIME__);
    LOCAL_PRINT(" -ResetBY: %s\r\n\r\n", getLastReasonReset(true));
}

void InitURTOS(UTask_t *taskTbl, uint8_t taskTblSize) {
    bannerIntro();

    taskManager.taskTbl = taskTbl;
    taskManager.taskTblSize = taskTblSize;
    LOCAL_LOG("Task table dataLen: %d", taskManager.taskTblSize);
    ASSERT_PARAM(taskManager.taskTbl != NULL && taskManager.taskTblSize != 0);

    semaSyncTasksStarted = xSemaphoreCreateCounting(taskManager.taskTblSize, taskManager.taskTblSize);
    ASSERT_PARAM(semaSyncTasksStarted != NULL);

    InitUEvents();

    for (uint8_t id = 0; id < taskManager.taskTblSize; ++id) {
        ASSERT_PARAM(
            pdFAIL != xTaskCreate(taskManager.taskTbl[id].onHandle,
                                  taskManager.taskTbl[id].desc,
								  taskManager.taskTbl[id].depth,
                                  NULL,
                                  taskManager.taskTbl[id].rank,
                                  &taskManager.taskTbl[id].CreateTask
                                ));
        taskManager.taskTbl[id].event = EVENT_MSG_NULL;

        LOCAL_LOG("[TaskCreate] %s\tid: %d\tStackSize: %d\tPrio: %d\tCb: 0x%p",
                taskManager.taskTbl[id].desc,
                taskManager.taskTbl[id].tId,
                taskManager.taskTbl[id].depth,
                taskManager.taskTbl[id].rank,
                taskManager.taskTbl[id].onHandle
                );
    }
}

void waitAllTasksStarted() {
    ASSERT_PARAM(pdFAIL != xSemaphoreTake(semaSyncTasksStarted, 10));

    while (uxSemaphoreGetCount(semaSyncTasksStarted) > 0) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static inline void enQueueEvent(UEvent_t *enq) {
    ASSERT_PARAM(enq != EVENT_MSG_NULL);

    ENTRY_CRITICAL();

    if (taskManager.taskTbl[enq->destTaskId].event == EVENT_MSG_NULL) {
        taskManager.taskTbl[enq->destTaskId].event = enq;
    }
    else {
        UEvent_t *lastEvent = taskManager.taskTbl[enq->destTaskId].event;

        /* Traverse until meeting last event of task */
        while (lastEvent->link != EVENT_MSG_NULL) {
            lastEvent = lastEvent->link;
        }
        lastEvent->link = enq;
    }

    EXIT_CRITICAL();
}

void taskEnqueueMessage(uint8_t destTaskId, uint8_t signal, uint8_t *data, uint32_t dataLen) {
    UEvent_t *e;
    
    e = CreateUEvent();
    ASSERT_PARAM(EVENT_MSG_NULL != e);

    e->destTaskId = destTaskId;
    e->signal = signal;
    if (data != NULL) {
        setUEventData(e, data, dataLen);
    }

    enQueueEvent(e);
}

bool taskDequeueMessage(uint8_t srcTaskId, UEvent_t *deq) {
    ASSERT_PARAM(srcTaskId < taskManager.taskTblSize);

    bool hasMessage = false;

    ENTRY_CRITICAL();

    if (taskManager.taskTbl[srcTaskId].event != EVENT_MSG_NULL) {
        LOCAL_DBG("[MSG]");
        LOCAL_DBG(" -DestTaskId: %d", taskManager.taskTbl[srcTaskId].event->destTaskId);
        LOCAL_DBG(" -signal: %d", taskManager.taskTbl[srcTaskId].event->signal);

        UEvent_t *event = taskManager.taskTbl[srcTaskId].event;
        memcpy(deq, taskManager.taskTbl[srcTaskId].event, sizeof(UEvent_t));
        taskManager.taskTbl[srcTaskId].event = taskManager.taskTbl[srcTaskId].event->link;
        EraseUEvent(event);
        hasMessage = true;
    }

    EXIT_CRITICAL();

    if (!hasMessage) {
        vTaskDelay(20);
    }

    return hasMessage;
}

static void localSoftTimerRun(TimerHandle_t th) {
    UTimer_t *t = (UTimer_t*)pvTimerGetTimerID(th);

    ASSERT_PARAM(t != EVENT_TIMER_NULL);
    LOCAL_DBG("[SoftEventTimer]");
    LOCAL_DBG(" -DestTaskId: %d", t->destTaskId);
    LOCAL_DBG(" -signal: %d", t->signal);

    taskEnqueueMessage(t->destTaskId, t->signal, NULL, 0);
}

void timerSet(uint8_t destTaskId, uint8_t signal, uint32_t period, bool isReload) {
    ASSERT_PARAM(destTaskId < taskManager.taskTblSize);

    UTimer_t *t = EVENT_TIMER_NULL;

    t = CreateUTimer();
    ASSERT_PARAM(t != EVENT_TIMER_NULL);

    t->signal = signal;
    t->destTaskId = destTaskId;
    t->CreateTimer = xTimerCreate("SoftTimer",
                                  pdMS_TO_TICKS(period),
                                  (isReload) ? pdTRUE : pdFALSE,
                                  (void*)t,
                                  localSoftTimerRun);

    ASSERT_PARAM(NULL != t->CreateTimer);                                
    xTimerStart(t->CreateTimer, pdMS_TO_TICKS(100));
}

void timerRemove(uint8_t destTaskId, uint8_t signal) {
    UTimer_t t;

    t.signal = signal;
    t.destTaskId = destTaskId;
    t.CreateTimer = NULL;
    EraseUTimer(&t);
    
    if (t.CreateTimer != NULL) {
        ASSERT_PARAM(pdPASS == xTimerDelete(t.CreateTimer, pdMS_TO_TICKS(100)));
    }
}
