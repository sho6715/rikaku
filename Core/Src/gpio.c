/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(BATT_LED_GPIO_Port, BATT_LED_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED4_GPIO_Port, LED4_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SEN_RF_GPIO_Port, SEN_RF_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SEN_R_GPIO_Port, SEN_R_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED3_GPIO_Port, LED3_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SEN_L_GPIO_Port, SEN_L_Pin);

  /**/
  LL_GPIO_ResetOutputPin(SEN_LF_GPIO_Port, SEN_LF_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED0_GPIO_Port, LED0_Pin);

  /**/
  LL_GPIO_ResetOutputPin(MOT0_GPIO_Port, MOT0_Pin);

  /**/
  LL_GPIO_ResetOutputPin(MOT1_GPIO_Port, MOT1_Pin);

  /**/
  LL_GPIO_SetOutputPin(CS_gyro_GPIO_Port, CS_gyro_Pin);

  /**/
  GPIO_InitStruct.Pin = SW0_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(SW0_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SW1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(SW1_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BATT_LED_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(BATT_LED_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED4_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED4_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SEN_RF_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SEN_RF_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SEN_R_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SEN_R_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED3_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED3_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = CS_gyro_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(CS_gyro_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SEN_L_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SEN_L_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SEN_LF_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SEN_LF_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED0_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = MOT0_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(MOT0_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = MOT1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(MOT1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
void SetLED(uint8_t data){
	if((data&0x01)==0x01){ 	LL_GPIO_SetOutputPin(LED0_GPIO_Port,LED0_Pin);
	}else{					LL_GPIO_ResetOutputPin(LED0_GPIO_Port,LED0_Pin);
	}
	if((data&0x02)==0x02){ 	LL_GPIO_SetOutputPin(LED1_GPIO_Port,LED1_Pin);
	}else{					LL_GPIO_ResetOutputPin(LED1_GPIO_Port,LED1_Pin);
	}
	if((data&0x04)==0x04){ 	LL_GPIO_SetOutputPin(LED2_GPIO_Port,LED2_Pin);
	}else{					LL_GPIO_ResetOutputPin(LED2_GPIO_Port,LED2_Pin);
	}
	if((data&0x08)==0x08){ 	LL_GPIO_SetOutputPin(LED3_GPIO_Port,LED3_Pin);
	}else{					LL_GPIO_ResetOutputPin(LED3_GPIO_Port,LED3_Pin);
	}
	if((data&0x10)==0x10){ 	LL_GPIO_SetOutputPin(LED4_GPIO_Port,LED4_Pin);
	}else{					LL_GPIO_ResetOutputPin(LED4_GPIO_Port,LED4_Pin);
	}
}

void SetBatLED(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(BATT_LED_GPIO_Port,BATT_LED_Pin);
	else			LL_GPIO_ResetOutputPin(BATT_LED_GPIO_Port,BATT_LED_Pin);
}

int8_t SW_IsOn_0(void){//virtical
	return LL_GPIO_IsInputPinSet(SW0_GPIO_Port,SW0_Pin);
}

int8_t SW_IsOn_1(void){//horizontal
	return LL_GPIO_IsInputPinSet(SW1_GPIO_Port,SW1_Pin);
}

void Set_SenFL(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(SEN_LF_GPIO_Port,SEN_LF_Pin);
	else			LL_GPIO_ResetOutputPin(SEN_LF_GPIO_Port,SEN_LF_Pin);
}

void Set_SenSL(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(SEN_L_GPIO_Port,SEN_L_Pin);
	else			LL_GPIO_ResetOutputPin(SEN_L_GPIO_Port,SEN_L_Pin);
}

void Set_SenSR(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(SEN_R_GPIO_Port,SEN_R_Pin);
	else			LL_GPIO_ResetOutputPin(SEN_R_GPIO_Port,SEN_R_Pin);
}

void Set_SenFR(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(SEN_RF_GPIO_Port,SEN_RF_Pin);
	else			LL_GPIO_ResetOutputPin(SEN_RF_GPIO_Port,SEN_RF_Pin);
}

void Set_MOT0(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(MOT0_GPIO_Port,MOT0_Pin);
	else			LL_GPIO_ResetOutputPin(MOT0_GPIO_Port,MOT0_Pin);
}

void Set_MOT1(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(MOT1_GPIO_Port,MOT1_Pin);
	else			LL_GPIO_ResetOutputPin(MOT1_GPIO_Port,MOT1_Pin);
}
/*
void Set_CS_L(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(CS_enL_GPIO_Port,CS_enL_Pin);
	else			LL_GPIO_ResetOutputPin(CS_enL_GPIO_Port,CS_enL_Pin);
}

void Set_CS_R(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(CS_enR_GPIO_Port,CS_enR_Pin);
	else			LL_GPIO_ResetOutputPin(CS_enR_GPIO_Port,CS_enR_Pin);
}
void Set_CS_gyro(uint8_t data){
	if(data == 1)	LL_GPIO_SetOutputPin(CS_gyro_GPIO_Port,CS_gyro_Pin);
	else			LL_GPIO_ResetOutputPin(CS_gyro_GPIO_Port,CS_gyro_Pin);
}
*/
/* USER CODE END 2 */
