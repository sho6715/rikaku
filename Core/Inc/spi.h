/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SPI2_Init(void);

/* USER CODE BEGIN Prototypes */
void DMA1_Channel3_Init(void);
void DMA1_Channel4_Init(void);
void SPI2_Start(void);
void SPI2_DMA_Communication(uint8_t length);
void SPI2_DMA1_ReceiveComplete_Callback(void);
void SPI2_DMA1_TransmitComplete_Callback(void);
uint8_t Get_SPI2ReciveData(uint8_t num);
void SetSPI2TransmitData(uint8_t num, uint8_t data);
/*
void SPI1_Start_test(void);
void SPI_Communication(SPI_TypeDef *SPIx, uint8_t *tx_data,uint8_t *rx_data, uint8_t length, GPIO_TypeDef *GPIOx, uint32_t CS_Pin);
void MPU9250_whoami(void);
*/
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

