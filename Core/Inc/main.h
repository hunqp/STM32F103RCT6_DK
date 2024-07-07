/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI_W5500_NSS_Pin GPIO_PIN_4
#define SPI_W5500_NSS_GPIO_Port GPIOA
#define SPI_W5500_SCK_Pin GPIO_PIN_5
#define SPI_W5500_SCK_GPIO_Port GPIOA
#define SPI_W5500_MISO_Pin GPIO_PIN_6
#define SPI_W5500_MISO_GPIO_Port GPIOA
#define SPI_W5500_MOSI_Pin GPIO_PIN_7
#define SPI_W5500_MOSI_GPIO_Port GPIOA
#define W5500_INT_Pin GPIO_PIN_10
#define W5500_INT_GPIO_Port GPIOB
#define W5500_INT_EXTI_IRQn EXTI15_10_IRQn
#define W5500_RST_Pin GPIO_PIN_11
#define W5500_RST_GPIO_Port GPIOB
#define W5500_EN_Pin GPIO_PIN_12
#define W5500_EN_GPIO_Port GPIOB
#define CM4_PWR_Pin GPIO_PIN_13
#define CM4_PWR_GPIO_Port GPIOB
#define LED_LIFE_Pin GPIO_PIN_8
#define LED_LIFE_GPIO_Port GPIOA
#define CONSOLE_DEBUG_RX_Pin GPIO_PIN_9
#define CONSOLE_DEBUG_RX_GPIO_Port GPIOA
#define CONSOLE_DEBUG_TX_Pin GPIO_PIN_10
#define CONSOLE_DEBUG_TX_GPIO_Port GPIOA
#define NX_POWER_C_Pin GPIO_PIN_8
#define NX_POWER_C_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define DHCP_UNITEST_ENABLE   0
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
