#ifndef __APP_DBG_H
#define __APP_DBG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\e[1;34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define APP_DBG_EN      (1)
#define APP_LOG_EN      (1)
#define APP_DBG_SIG_EN  (1)

#define APP_PRINT					printf

#if APP_DBG_EN
#define APP_DBG(tag, fmt, ...)     APP_PRINT(KMAG "[" tag "] " KNRM fmt "\r\n", ##__VA_ARGS__)
#else
#define APP_DBG(tag, fmt, ...)
#endif

#if APP_LOG_EN
#define APP_LOG(tag, fmt, ...)     APP_PRINT("[" tag "] " fmt "\r\n", ##__VA_ARGS__)
#else
#define APP_LOG(tag, fmt, ...)
#endif

#if APP_DBG_SIG_EN
#define APP_DBG_SIG(tag, fmt, ...)  APP_PRINT(KMAG "[SIG] " KGRN tag " -> " KYEL fmt "\r\n" KNRM, ##__VA_ARGS__)
#else
#define APP_DBG_SIG(tag, fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif
