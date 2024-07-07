#include "urtos_events.h"

static UEvent_t uEventsBox[EVENT_MSG_FULL_SIZE];
static UTimer_t uTimersBox[EVENT_TIMER_FULL_SIZE];
static UEvent_t* pointToFreeUEvents;
static uint32_t uEventsBoxHasUsed;
static uint32_t uTimersBoxHasUsed;

void InitUEvents() {
    /*----------------------------------*
     *  SETUP : Microevents box
     *----------------------------------*/
    pointToFreeUEvents = (UEvent_t*)uEventsBox;

    for (int id = 0; id < EVENT_MSG_FULL_SIZE; id++) {
        uEventsBox[id].message.data = NULL;
        uEventsBox[id].message.dataLen = 0;
    	uEventsBox[id].link = &uEventsBox[id + 1];
    }
    uEventsBox[EVENT_MSG_FULL_SIZE - 1].link = EVENT_MSG_NULL;

    uEventsBoxHasUsed = 0;

    /*----------------------------------*
     *  SETUP : Microtimers box
     *----------------------------------*/
    for (int id = 0; id < EVENT_TIMER_FULL_SIZE; id++) {
        uTimersBox[id].isUsed = false;
        uTimersBox[id].link = &uTimersBox[id + 1];
    }
    uTimersBox[EVENT_TIMER_FULL_SIZE - 1].link = EVENT_TIMER_NULL;

    uTimersBoxHasUsed = 0;
}

uint8_t* getUEventData(UEvent_t *e) {
    return (e == NULL) ? NULL : (e->message.data);
}

uint32_t getUEventDataLen(UEvent_t *e) {
    return (e == NULL) ? 0 : (e->message.dataLen);
}

UEvent_t* CreateUEvent() {
    UEvent_t *e;

	e = pointToFreeUEvents;
	if (e != EVENT_MSG_NULL) {
        pointToFreeUEvents = e->link;
        ++(uEventsBoxHasUsed);
        e->link = EVENT_MSG_NULL;
    }
  
	return e;
}

void EraseUEvent(UEvent_t *e) {
    if (e->message.data != NULL) {
        vPortFree(e->message.data);
        e->message.data = NULL;
    }
    e->link = pointToFreeUEvents;
    pointToFreeUEvents = e;
    --(uEventsBoxHasUsed);
}

void setUEventData(UEvent_t *e,  uint8_t *data, uint32_t dataLen) {
    e->message.data = (uint8_t*)pvPortMalloc(dataLen);
    e->message.dataLen = dataLen;
    memset(e->message.data, 0, e->message.dataLen);
    memcpy(e->message.data, data, dataLen);
}

uint32_t getUEventsUsed() {
    return uEventsBoxHasUsed;
}

uint32_t getUEventsMax() {
    return EVENT_MSG_FULL_SIZE;
}

UTimer_t* CreateUTimer() {
    UTimer_t *t;

    t = uTimersBox;
    while (t != EVENT_TIMER_NULL) {
        if (t->isUsed == false) {
            t->isUsed = true;
            ++(uEventsBoxHasUsed);
            break;
        }
        t = t->link;
    }

	return t;
}

void EraseUTimer(UTimer_t *delTimer) {
    UTimer_t *t;

    t = uTimersBox;
    while (t != EVENT_TIMER_NULL) {
        if (t->destTaskId == delTimer->destTaskId && t->signal == delTimer->signal) {
            t->isUsed = false;
            memcpy(delTimer, t, sizeof(UTimer_t));
            --(uTimersBoxHasUsed);
            break;
        }
        t = t->link;
    }
}

uint32_t getUTimersUsed() {
    return uTimersBoxHasUsed;
}

uint32_t getUTimersMax() {
    return EVENT_TIMER_FULL_SIZE;
}
