/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* SPI2 init function */
void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  /**SPI2 GPIO Configuration
  PB13   ------> SPI2_SCK
  PB14   ------> SPI2_MISO
  PB15   ------> SPI2_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI2 DMA Init */

  /* SPI2_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_SPI2_RX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);

  /* SPI2_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_4, LL_DMAMUX_REQ_SPI2_TX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MDATAALIGN_BYTE);

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPI2);
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/* USER CODE BEGIN 1 */
//uint8_t rx_flag = 0;
//uint8_t tx_flag = 0;

void DMA1_Channel3_Init(void)
{
	LL_DMA_EnableIT_TC(DMA1,LL_DMA_CHANNEL_3);
	LL_DMA_EnableIT_TE(DMA1,LL_DMA_CHANNEL_3);
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_3);
}

void DMA1_Channel4_Init(void)
{
	LL_DMA_EnableIT_TC(DMA1,LL_DMA_CHANNEL_4);
	LL_DMA_EnableIT_TE(DMA1,LL_DMA_CHANNEL_4);
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_4);
}

#define SPI2_DATA_BUFFR_SIZE	((uint32_t)3)

uint8_t SPI2ReciveData[SPI2_DATA_BUFFR_SIZE];
uint8_t SPI2TransmitData[SPI2_DATA_BUFFR_SIZE];

void SPI2_Start(void)
{
	LL_SPI_SetRxFIFOThreshold(SPI2, LL_SPI_RX_FIFO_TH_QUARTER);
	DMA1_Channel3_Init();
	DMA1_Channel4_Init();
	LL_SPI_EnableDMAReq_RX(SPI2);
	LL_SPI_EnableDMAReq_TX(SPI2);

	LL_SPI_Enable(SPI2);
}

void SPI2_DMA_Communication(uint8_t length)
{
  LL_GPIO_ResetOutputPin(CS_gyro_GPIO_Port, CS_gyro_Pin);

	if( LL_SPI_IsActiveFlag_RXNE(SPI2) == SET)LL_SPI_ReceiveData8(SPI2);
	if( LL_SPI_IsEnabled(SPI2) == RESET) LL_SPI_Enable(SPI2);

	LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_4,(uint32_t)SPI2TransmitData,
							LL_SPI_DMA_GetRegAddr(SPI2),LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_4,length);

	LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_3,LL_SPI_DMA_GetRegAddr(SPI2),
							(uint32_t)SPI2ReciveData,LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_3,length);

	LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_4);
	LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_3);
/*
	while(tx_flag != 1);
	tx_flag = 0;
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_2);
	while(rx_flag != 1);
	rx_flag = 0;
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_1);
	LL_GPIO_SetOutputPin(GPIOx,CS_Pin);
*/
}

void SPI2_DMA1_ReceiveComplete_Callback(void)
{
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_3);
	LL_GPIO_SetOutputPin(CS_gyro_GPIO_Port, CS_gyro_Pin);
//	rx_flag = 1;
}

void SPI2_DMA1_TransmitComplete_Callback(void)
{
	LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_4);
//	tx_flag = 1;
}

uint8_t Get_SPI2ReciveData(uint8_t num){
	return SPI2ReciveData[num];
}

void SetSPI2TransmitData(uint8_t num, uint8_t data){
	SPI2TransmitData[num] = data;
}
/* USER CODE END 1 */
