/*
 * search.h
 *
 *  Created on: 2021/02/04
 *      Author: shohe
 */

#ifndef INC_SEARCH_SEARCH_H_
#define INC_SEARCH_SEARCH_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#include "Inc/gpio.h"
#include "Inc/hal/run.h"
#include "Inc/search/map_cmd.h"
#include "Inc/search/map_flash.h"
#include "Inc/hal/log.h"
#include "Inc/hal/mode.h"
#include "Inc/queue.h"
#else
#include "glob.h"
#include "gpio.h"
#include "hal/run.h"
#include "search/map_cmd.h"
#include "search/map_flash.h"
#include "hal/log.h"
#include "hal/mode.h"
#include "queue.h"
#endif

/* 探索方�? */
typedef enum{
	CONTOUR_SYSTEM =0,			// 等高線MAP�?
	MAP_SEARCH_TYPE_MAX,
}enMAP_SEARCH_TYPE;

/* 探索方�? */
typedef enum{
	SEARCH =0,			// 探索
	BEST_WAY,			// 最短
	MAP_ACT_MODE_MAX,
}enMAP_ACT_MODE;

/* 探索動�? */
typedef enum{
	SEARCH_TURN =0,		// �?信地旋回探索
	SEARCH_SLA,		// スラロー�?探索
	SEARCH_SKEW,		// 斜め探索
	SEARCH_RETURN,
	SEARCH_MAX,
}enSEARCH_MODE;

extern bool	front_wall_miss;

extern uint8_t	Return_X;
extern uint8_t	Return_Y;

extern uint16_t	us_Cmap[MAP_Y_SIZE][MAP_X_SIZE];				//< 等高�? �?ータ
extern uint8_t		g_SysMap[ MAP_Y_SIZE ][ MAP_X_SIZE ];			///< 迷路�?報

extern uint8_t		g_MapDirection[MAP_Y_SIZE][MAP_X_SIZE];			///< 迷路情報

extern bool			SearchFlag;

bool Is_Known_Accel(void);
void MAP_init( void );
void MAP_Goal_init( void );
void MAP_Goal_change_x( void );
void MAP_Goal_change_y( void );
void MAP_2Log( void );
void MAP_clearMap( void );
void MAP_setPos( uint8_t uc_x, uint8_t uc_y, enMAP_HEAD_DIR en_dir );
void MAP_searchGoal( uint8_t uc_trgX, uint8_t uc_trgY, enMAP_ACT_MODE en_type, enSEARCH_MODE en_search );
void MAP_makeContourMap( uint8_t uc_goalX, uint8_t uc_goalY, enMAP_ACT_MODE en_type );
void MAP_makeContourMap_run( uint8_t uc_goalX,uint8_t uc_goalY,	enMAP_ACT_MODE	en_type);
void MAP_actGoalLED( void );
void MAP_showLog( void );

void MAP_ClearMapData( void );
void MAP_Goalsize(int size);

//TKR
void MAP_searchGoalKnown(uint8_t uc_trgX, uint8_t uc_trgY, enMAP_ACT_MODE en_type,enSEARCH_MODE	en_search);

void  MAP_makeContourMap_dijkstra_modoki(uint8_t uc_goalX, uint8_t uc_goalY, enMAP_ACT_MODE	en_type	);


#endif /* INC_SEARCH_SEARCH_H_ */
