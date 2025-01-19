/*
 * flash.h
 *
 *  Created on: 2021/02/06
 *      Author: shohe
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

#define FLASH_KEY1		0x45670123U
#define FLASH_KEY2		0xCDEF89ABU

#define sta_add_127 0x0807F800
#define end_add_127 0x0807FFFF

__STATIC_INLINE void FLASH_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
}

__STATIC_INLINE void FLASH_Unlock(void)
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}

void FLASH_Init(void);
void FLASH_WaitBusy(void);
void FLASH_Erase(uint8_t page);
void FLASH_WriteByte(uint32_t address, uint64_t data);
void FLASH_WriteData(uint8_t page, uint32_t address, uint64_t* data, uint32_t size);
void FLASH_ReadData(uint32_t address, uint64_t* data, uint32_t size);


#endif /* INC_FLASH_H_ */
