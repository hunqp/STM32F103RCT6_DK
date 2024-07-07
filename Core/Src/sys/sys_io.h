#ifndef __SYS_IO_H
#define __SYS_IO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "gpio.h"
#include "iwdg.h"
#include "usart.h"
#include "spi.h"

#define SYSTEM_ENABLE_IRQ()       __enable_irq()
#define SYSTEM_DISABLE_IRQ()      __disable_irq()

extern void blinkLedLife();
extern void W5x00_Reset();
extern void W5x00_SetCSHigh();
extern void W5x00_SetCSLow();
extern void W5x00_ReadData(uint8_t *data, uint16_t size);
extern void W5x00_SendData(uint8_t *data, uint16_t size);
extern uint8_t W5x00_ReadByte(void);
extern void W5x00_SendByte(uint8_t byte);

extern void softReset();
extern void rstHardWatchdog();
extern char* getLastReasonReset(bool clear);
extern void delayInMicroseconds(uint32_t us);
extern void delayInMilliseconds(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_IO_H */
