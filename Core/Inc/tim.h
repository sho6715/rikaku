/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define ENC_RESET_VAL  (65536/2)
/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);
void MX_TIM6_Init(void);
void MX_TIM7_Init(void);
void MX_TIM8_Init(void);
void MX_TIM15_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);
void MX_TIM6_Init(void);
void MX_TIM8_Init(void);

/* USER CODE BEGIN Prototypes */
void Enable_TIM2(void);
void Disable_TIM2(void);
void Enable_TIM3(void);
void Disable_TIM3(void);
void Enable_TIM1(void);
void Disable_TIM1(void);
void Enable_TIM5(void);
void Disable_TIM5(void);
void Set_DutyTIM2(uint16_t duty);
void Set_DutyTIM3(uint16_t duty);
void Clear_CounterTIM2(void);
void Clear_CounterTIM3(void);
void Clear_CounterTIM1(void);
void Clear_CounterTIM5(void);
void Clear_CounterTIM8(void);
void Set_DutyTIM8(uint16_t duty);
uint16_t Get_CounterTIM1(void);
uint16_t Get_CounterTIM5(void);
void TIMER_init(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

