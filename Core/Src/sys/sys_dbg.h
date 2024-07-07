#ifndef __SYS_DBG_H
#define __SYS_DBG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "sys_io.h"

#define FATAL_REBOOT 	0

#define RED_COLOR       "\x1B[31m"
#define RESET_COLOR     "\x1B[0m"

#define SYS_PRINT       printf

#define FATAL(msg)                                       \
    do                                                   \
    {                                                    \
        SYSTEM_DISABLE_IRQ();                            \
        SYS_PRINT("\r\n");                               \
        SYS_PRINT(RED_COLOR "[FATAL] " RESET_COLOR msg "\r\n"); \
        if (FATAL_REBOOT)                                \
        {                                                \
            softReset();			                     \
        }                                                \
        do                                               \
        {                                                \
            blinkLedLife(); 							 \
            rstHardWatchdog();                           \
            delayInMilliseconds(30);                     \
        } while (1);                                     \
    } while (0);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_DBG_H */
