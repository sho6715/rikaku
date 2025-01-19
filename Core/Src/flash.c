/*
 * flash.c
 *
 *  Created on: 2021/02/06
 *      Author: shohe
 */


#include "flash.h"

#include <string.h>
#include <stdint.h>

#define FLASH_TYPEPROGRAM_BYTE	0x00000000U

//#define FLASH

void FLASH_WaitBusy(void)
{
	while(((FLASH->SR & FLASH_SR_BSY)== FLASH_SR_BSY)==1);
//	while((FLASH->SR & FLASH_SR_BSY)== FLASH_SR_BSY);
}

void FLASH_Erase(uint8_t page)
{
	FLASH_WaitBusy();

	FLASH->SR &= 0x00000000;	//error clear?
	FLASH->CR &= FLASH_TYPEPROGRAM_BYTE;
	FLASH->CR |= (FLASH_CR_BKER);	//set bank2
	FLASH->CR |= FLASH_CR_PER;
	FLASH->CR |= (FLASH_CR_PNB & (page<<3));
	FLASH->CR |= FLASH_CR_STRT;
	FLASH_WaitBusy();
}

void FLASH_WriteByte(uint32_t address, uint64_t data)
{
	uint32_t data2 = (uint32_t)(data >> 32);
	FLASH_WaitBusy();
	FLASH->SR &= 0x00000000;	//error clear
	FLASH->CR &= FLASH_TYPEPROGRAM_BYTE;
	FLASH->CR |= FLASH_CR_PG;

	*(__IO uint32_t*)address = (uint32_t)data;
	*(__IO uint32_t*)(address+4) = data2;
	FLASH_WaitBusy();

	FLASH->CR &= ~(FLASH_CR_PG);
}

void FLASH_WriteData(uint8_t page,uint32_t address, uint64_t* data, uint32_t size)
{
	FLASH_Unlock();

	FLASH_Erase(page);

	do {
		FLASH_WriteByte(address, *data);
		address+=8;
		data++;
		size -=8;
	}while(size > 0);

	FLASH_Lock();
}

void FLASH_ReadData(uint32_t address, uint64_t* data, uint32_t size)
{
	memcpy(data, (uint8_t*)address,size);
}
