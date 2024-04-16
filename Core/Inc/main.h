/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

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
#define SD_MOSI_Pin LL_GPIO_PIN_7
#define SD_MOSI_GPIO_Port GPIOB
#define ENCODER_B_Pin LL_GPIO_PIN_9
#define ENCODER_B_GPIO_Port GPIOB
#define ENCODER_B_EXTI_IRQn EXTI4_15_IRQn
#define ENCODER_C_Pin LL_GPIO_PIN_15
#define ENCODER_C_GPIO_Port GPIOC
#define ENCODER_C_EXTI_IRQn EXTI4_15_IRQn
#define SD_SCK_Pin LL_GPIO_PIN_0
#define SD_SCK_GPIO_Port GPIOA
#define LCD_SCL_Pin LL_GPIO_PIN_1
#define LCD_SCL_GPIO_Port GPIOA
#define LCD_SDA_Pin LL_GPIO_PIN_2
#define LCD_SDA_GPIO_Port GPIOA
#define LCD_DC_Pin LL_GPIO_PIN_3
#define LCD_DC_GPIO_Port GPIOA
#define LED_INDICTR_Pin LL_GPIO_PIN_4
#define LED_INDICTR_GPIO_Port GPIOA
#define LCD_CS_Pin LL_GPIO_PIN_5
#define LCD_CS_GPIO_Port GPIOA
#define LCD_BKLT_Pin LL_GPIO_PIN_6
#define LCD_BKLT_GPIO_Port GPIOA
#define ENCODER_A_Pin LL_GPIO_PIN_7
#define ENCODER_A_GPIO_Port GPIOA
#define ENCODER_A_EXTI_IRQn EXTI4_15_IRQn
#define SD_MISO_Pin LL_GPIO_PIN_2
#define SD_MISO_GPIO_Port GPIOB
#define LED_LENT_0_Pin LL_GPIO_PIN_11
#define LED_LENT_0_GPIO_Port GPIOA
#define LCD_RES_Pin LL_GPIO_PIN_12
#define LCD_RES_GPIO_Port GPIOA
#define SD_CS_Pin LL_GPIO_PIN_6
#define SD_CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
