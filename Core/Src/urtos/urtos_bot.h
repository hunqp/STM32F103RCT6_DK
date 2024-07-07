#ifndef __MICRORTOSBOT_H
#define __MICRORTOSBOT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "urtos_events.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "message_buffer.h"
#include "event_groups.h"

#define BUILD_RELEASE			(0)
#define CYBORG					"Dynamo"

#if BUILD_RELEASE
    #define BUILD   (const char*)"RELEASE"
    #define VERSION	(const char*)"v1.1"
    #define ASSERT	(0)
#else
    #define BUILD   (const char*)"DEBUG"
    #define VERSION (const char*)"v1.0"
    #define ASSERT	(1)
#endif

#define URTOS_DEFINE_SIGNAL		(10)

#define ENTRY_CRITICAL()		portDISABLE_INTERRUPTS()
#define EXIT_CRITICAL()			portENABLE_INTERRUPTS()

typedef enum {
	TASK_RANK_1 = 5,
	TASK_RANK_2,
	TASK_RANK_3,
	TASK_RANK_4,
	TASK_RANK_5,
	TASK_RANK_6,
	TASK_RANK_7,
} URTOS_TASK_RANK;

typedef struct {
	uint8_t tId;
	void (*onHandle)(void *);
	uint16_t depth;
	URTOS_TASK_RANK rank;
	const char *const desc;
	TaskHandle_t CreateTask;
	UEvent_t *event;
} UTask_t;

extern void InitURTOS(UTask_t *taskTbl, uint8_t taskTblSize);
extern void waitAllTasksStarted();
extern void taskEnqueueMessage(uint8_t destTaskId, uint8_t signal, uint8_t *data, uint32_t dataLen);
extern bool taskDequeueMessage(uint8_t srcTaskId, UEvent_t *e);
extern void timerSet(uint8_t destTaskId, uint8_t signal, uint32_t period, bool isReload);
extern void timerRemove(uint8_t destTaskId, uint8_t signal);

extern void assertParams(bool IsTrue, const char *file, uint32_t line);

#define ASSERT_PARAM(assert)    assertParams(assert, __FILE__, __LINE__)

#define TaskPostPureMessage(i,s)		taskEnqueueMessage(i, s, NULL, 0)
#define TaskPostDynamicMessage(i,s,d,l)	taskEnqueueMessage(i, s, d, l)
#define TimerSet(i,s,p,r)				timerSet(i, s, p, r)
#define TimerRemove(i,s)				timerRemove(i, s)

#ifdef __cplusplus
}
#endif

#endif /* __MICRORTOSBOT_H */
