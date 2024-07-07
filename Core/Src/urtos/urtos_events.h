#ifndef __MICRORTOSEVENTS_H
#define __MICRORTOSEVENTS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "timers.h"

#ifndef EVENT_MSG_FULL_SIZE
#define EVENT_MSG_FULL_SIZE         ( 16 )
#endif

#ifndef EVENT_TIMER_FULL_SIZE
#define EVENT_TIMER_FULL_SIZE       ( 8 )
#endif

#define EVENT_MSG_NULL              (struct t_UEvent*)NULL
#define EVENT_TIMER_NULL            (struct t_UTimer*)NULL

typedef struct {
    uint8_t *data;
    uint32_t dataLen;
} UMessage_t;

typedef struct t_UEvent {
    struct t_UEvent *link;
    uint8_t destTaskId;
    uint8_t signal;
    UMessage_t message;
} UEvent_t;

typedef struct t_UTimer {
    struct t_UTimer *link;
    uint8_t destTaskId;
    uint8_t signal;
    bool isUsed;
    TimerHandle_t CreateTimer;
} UTimer_t;

extern void InitUEvents();
extern uint8_t* getUEventData();
extern uint32_t getUEventDataLen();

/*----------------------------------*
 * FUNCTION PROTOTYPE : Micro events
 *----------------------------------*/
extern UEvent_t* CreateUEvent();
extern void EraseUEvent(UEvent_t*);
extern void setUEventData(UEvent_t*,  uint8_t*, uint32_t);
extern uint32_t getUEventsUsed();
extern uint32_t getUEventsMax();

/*----------------------------------*
 * FUNCTION PROTOTYPE : Micro timers
 *----------------------------------*/
extern UTimer_t* CreateUTimer();
extern void EraseUTimer(UTimer_t*);
extern uint32_t getUTimersUsed();
extern uint32_t getUTimersMax();

#ifdef __cplusplus
}
#endif

#endif /* __MICRORTOSEVENTS_H */
