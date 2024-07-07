#ifndef __TASK_CONSOLE_H
#define __TASK_CONSOLE_H

#include "cmd_line.h"

extern const Args_t lgnCmdTable[];
extern TaskHandle_t CreateTaskConsole;

extern void taskConsoleHandle(void *);

#endif
