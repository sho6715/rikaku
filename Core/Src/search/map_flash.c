/*
 * map_flash.c
 *
 *  Created on: 2021/02/06
 *      Author: shohe
 */


#include "search/map_flash.h"

void Map_Write(void)
{
	uint64_t *map_add;
	map_add = (uint64_t *)g_SysMap;
	
	//DataFlashイレース
//    FLASH_Erase(0x7F);  //reg127
	//マップデータをDataFlashに書き込む
    FLASH_WriteData(0x7F,(uint32_t)sta_add_127, map_add, 32*32);
}

void Map_Copy(void)
{
    uint64_t *map_add;
    map_add = (uint64_t *)&g_SysMap;
    uint32_t read_address = sta_add_127;

    for (uint16_t i = 0;i<MAP_X_SIZE*MAP_Y_SIZE;i += 8){
	    FLASH_ReadData(read_address, map_add, 8);
	    read_address +=8;
	    map_add++;
    }
}

void Map_Erase(void)
{
	FLASH_Unlock();
	FLASH_Erase(0x7F);
	FLASH_Lock();
}



