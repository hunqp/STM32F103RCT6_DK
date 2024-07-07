#ifndef __TASK_LIST_H
#define __TASK_LIST_H

#include "../urtos/urtos_bot.h"

#define TASK_SYSTEM_EN      (1)
#define TASK_CONSOLE_EN     (1)
#define TASK_CLOUD_EN       (1)
#define TASK_DMN_EN         (1)

enum {
#if TASK_SYSTEM_EN
    TASK_SYSTEM_ID,
#endif
#if TASK_CONSOLE_EN
    TASK_CONSOLE_ID,
#endif
#if TASK_CLOUD_EN
    TASK_CLOUD_ID,
#endif

#if TASK_DMN_EN
    TASK_DMN_ID,
#endif

    EOT_TASK_ID,
};

extern UTask_t taskList[];

#endif /* __TASK_LIST_H */
