/*
 * map_cmd.h
 *
 *  Created on: 2021/02/04
 *      Author: shohe
 */

#ifndef INC_SEARCH_MAP_CMD_H_
#define INC_SEARCH_MAP_CMD_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#include "Inc/search/search.h"
#else
#include "glob.h"
#include "search/search.h"
#endif

/* コマンド走行の種類 */
typedef enum{
	MAP_DRIVE_TURN =0,			// 超信地旋回
	MAP_DRIVE_SLA,				// スラローム
	MAP_DRIVE_SKEW,				// 斜め
	MAP_DRIVE_TYPE_MAX,
}enMAP_DRIVE_TYPE;

void MAP_makeCmdList( uint8_t uc_staX, uint8_t uc_staY, enMAP_HEAD_DIR en_staDir, uint8_t uc_endX, uint8_t uc_endY, enMAP_HEAD_DIR* en_endDir );
void MAP_makeSlaCmdList( void );
void MAP_makeSkewCmdList( void );
void MAP_drive( enMAP_DRIVE_TYPE en_driveType );
void MAP_setCmdPos( uint8_t uc_x, uint8_t uc_y, enMAP_HEAD_DIR en_dir );

void MAP_showCmdLog( void );
void MAP_searchCmdList(uint8_t uc_staX,uint8_t uc_staY,enMAP_HEAD_DIR en_staDir,uint8_t uc_endX,uint8_t uc_endY,enMAP_HEAD_DIR* en_endDir);

void MAP_makeCmdList_dijkstra_modoki(uint8_t uc_staX,uint8_t uc_staY,enMAP_HEAD_DIR en_staDir,uint8_t uc_endX,uint8_t uc_endY,enMAP_HEAD_DIR* en_endDir);

#endif /* INC_SEARCH_MAP_CMD_H_ */
