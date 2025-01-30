/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include "keyboard.h"
#include "encoder.h"
#include "ws2812b.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
extern volatile uint32_t millis; //счетчик системного времени, мс
extern ENCODER_Handler encoder1; //обработчик энкодера

//Обработка событий энкодера с эмуляцией нажатия кнопок для модуля Keyboard
static void EncoderEventToKeyboard(uint8_t event)
{
	uint32_t keys = 0;
	if (event == ENCODER_EV_FWD) {
		keys = 1 << KEYB_DOWN;
	}
	else if (event == ENCODER_EV_BCK) {
		keys = 1 << KEYB_UP;
	}
	else if (event == ENCODER_EV_BTN_CLICK) {
		keys = 1 << KEYB_RIGHT;
	}
	else if (event == ENCODER_EV_BTN_HOLD) {
		keys = 1 << KEYB_LEFT;
	}
	else if (event == ENCODER_EV_FWD_BTN) {
		keys = 1 << KEYB_F1;
	}
	else if (event == ENCODER_EV_BCK_BTN) {
		keys = 1 << KEYB_F2;
	}

	if (keys && (KEYB_count_key < KEYB_SIZE_BUFFER))
	{
		KEYB_key_buff[KEYB_count_key] = keys;
		KEYB_count_key++;
	}
}

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	millis++;
  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_7) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_7);
    /* USER CODE BEGIN LL_EXTI_LINE_7_FALLING */
    EncoderEventToKeyboard(EncoderUpdate(&encoder1, ENCODER_IRQ_A));
    /* USER CODE END LL_EXTI_LINE_7_FALLING */
  }
  if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_7) != RESET)
  {
    LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_7);
    /* USER CODE BEGIN LL_EXTI_LINE_7_RISING */
    EncoderEventToKeyboard(EncoderUpdate(&encoder1, ENCODER_IRQ_A));
    /* USER CODE END LL_EXTI_LINE_7_RISING */
  }
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_9) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_9);
    /* USER CODE BEGIN LL_EXTI_LINE_9_FALLING */
    EncoderEventToKeyboard(EncoderUpdate(&encoder1, ENCODER_IRQ_B));
    /* USER CODE END LL_EXTI_LINE_9_FALLING */
  }
  if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_9) != RESET)
  {
    LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_9);
    /* USER CODE BEGIN LL_EXTI_LINE_9_RISING */
    EncoderEventToKeyboard(EncoderUpdate(&encoder1, ENCODER_IRQ_B));
    /* USER CODE END LL_EXTI_LINE_9_RISING */
  }
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_15) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_15);
    /* USER CODE BEGIN LL_EXTI_LINE_15_FALLING */
    EncoderEventToKeyboard(EncoderUpdate(&encoder1, ENCODER_IRQ_C));
    /* USER CODE END LL_EXTI_LINE_15_FALLING */
  }
  if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_15) != RESET)
  {
    LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_15);
    /* USER CODE BEGIN LL_EXTI_LINE_15_RISING */
    EncoderEventToKeyboard(EncoderUpdate(&encoder1, ENCODER_IRQ_C));
    /* USER CODE END LL_EXTI_LINE_15_RISING */
  }
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	Display_TC_Callback(DMA1, 1);
  /* USER CODE END DMA1_Channel1_IRQn 0 */

  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */

  /* USER CODE END DMA1_Channel2_3_IRQn 0 */

  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 4, channel 5 and DMAMUX1 interrupts.
  */
void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Ch4_5_DMAMUX1_OVR_IRQn 0 */
	LED_Callback(DMA1, LL_DMA_CHANNEL_4);
  /* USER CODE END DMA1_Ch4_5_DMAMUX1_OVR_IRQn 0 */

  /* USER CODE BEGIN DMA1_Ch4_5_DMAMUX1_OVR_IRQn 1 */

  /* USER CODE END DMA1_Ch4_5_DMAMUX1_OVR_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
