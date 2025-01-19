/*
 * map_flash.h
 *
 *  Created on: 2021/02/06
 *      Author: shohe
 */

#ifndef INC_SEARCH_MAP_FLASH_H_
#define INC_SEARCH_MAP_FLASH_H_

#ifdef _VSCODE
#include "Inc/flash.h"
#include "Inc/search/search.h"
#else
#include "flash.h"
#include "search/search.h"
#endif

void Map_Write(void);
void Map_Copy(void);
void Map_Erase(void);

#endif /* INC_SEARCH_MAP_FLASH_H_ */
