/*
 * search.c
 *
 *  Created on: 2021/02/04
 *      Author: shohe
 */


#include "search/search.h"

#define MAP_SMAP_MAX_VAL	( MAP_X_SIZE * MAP_Y_SIZE ) 			///< 等高線mapの最大値
#define MAP_SMAP_MAX_PRI_VAL	( MAP_SMAP_MAX_VAL * 4 )				///< 等高線mapの優先度最大値

#define MOVE_BACK_DIST		(0.215f)

extern uint8_t WallHitFlag;

enMAP_HEAD_DIR	en_Head;										///< マウスの進行方向 0:N 1:E 2:S 3:W
uint8_t		my;												///< マウスのＸ座標
uint8_t		mx;												///< マウスのＹ座標
float		f_MoveBackDist;									///< 壁当て動作で後退した距離[区画]
uint8_t		uc_SlaCnt = 0;									// スラローム連続回数
uint8_t		uc_back[ MAP_Y_SIZE ][ MAP_X_SIZE ];			// 迷路データ

uint8_t		GOAL_MAP_X;					//ゴール座標変更プログラム用ｘ
uint8_t		GOAL_MAP_Y;					//ゴール座標変更プログラム用y

uint8_t		GOAL_SIZE;
//等高線マップを更新を止めるための移動区画規定変数
uint8_t		uc_max_x = GOAL_MAP_X_DEF;
uint8_t		uc_max_y = GOAL_MAP_Y_DEF;

uint8_t			SLA_Count = 30;

//TKR
/* 既知区間加速 */
typedef struct{
	uint8_t	uc_strCnt;
	bool	bl_known;
}stMAP_KNOWN;

stMAP_KNOWN		st_Known = { 0,FALSE };

uint8_t NearWall = 0;

bool	front_wall_miss;

uint8_t	Return_X;
uint8_t	Return_Y;

uint16_t	us_Cmap[MAP_Y_SIZE][MAP_X_SIZE];				//< 等高�? �?ータ
uint8_t		g_SysMap[ MAP_Y_SIZE ][ MAP_X_SIZE ];			///< 迷路�?報

uint8_t		g_MapDirection[MAP_Y_SIZE][MAP_X_SIZE];			///< 迷路情報


bool			SearchFlag;

bool Is_Known_Accel(void){
	return st_Known.bl_known;
}

void MAP_init( void )
{
//	uint8_t uc_dummy[ MAP_Y_SIZE ][ MAP_X_SIZE ];			// 迷路データ

	/* 座標、向き、迷路情報を初期化 */
	en_Head		= NORTH;
	mx		= 0;
	my		= 0;
	MAP_clearMap();

	front_wall_miss = FALSE;
	
	/* 走行用のパラメータ */
	f_MoveBackDist = 0;
	uc_SlaCnt = 0;

}

void MAP_Goal_init( void )
{
	GOAL_MAP_X = GOAL_MAP_X_DEF;
	GOAL_MAP_Y = GOAL_MAP_Y_DEF;
}

void MAP_Goal_change_x( void )
{
//	LED4 = GOAL_MAP_X;
	while(1){
		if ( SW_ON == SW_IsOn_1() ){
			GOAL_MAP_X++;
			if (GOAL_MAP_X == 16){
				GOAL_MAP_X = 0;
			}
//			LED4 = GOAL_MAP_X;
			LL_mDelay(200);			// SWが離されるまで待つ
			printf("GOAL_x %d\r\n",GOAL_MAP_X);
		} 
		else if (( SW_ON == SW_IsOn_0() )||(TRUE == MODE_CheckExe())){
			SetLED(0x00);
			break;
		}
	}
}

void MAP_Goal_change_y( void )
{
//	LED4 = GOAL_MAP_Y;
	while(1){
		if ( SW_ON == SW_IsOn_1() ){
			GOAL_MAP_Y++;
			if (GOAL_MAP_Y == 16){
				GOAL_MAP_Y = 0;
			}
//			LED4 = GOAL_MAP_Y;
			LL_mDelay(200);			// SWが離されるまで待つ
			printf("GOAL_y %d\r\n",GOAL_MAP_Y);
		} 
		else if (( SW_ON == SW_IsOn_0() )||(TRUE == MODE_CheckExe())){
			SetLED(0x00);
			break;
		}
	}
}

void MAP_ClearMapData( void )
{
	/* 座標、向き、迷路情報を初期化 */
	en_Head		= NORTH;
	mx			= 0;
	my			= 0;
	MAP_clearMap();
	
	/* 走行用のパラメータ */
	f_MoveBackDist = 0;
	uc_SlaCnt = 0;

//	Storage_Clear( sizeof(g_SysMap), ADR_MAP );			// データセーブ
}

void MAP_setPos( uint8_t uc_x, uint8_t uc_y, enMAP_HEAD_DIR en_dir )
{
	mx		= uc_x;
	my		= uc_y;
	en_Head		= en_dir;
	
	MAP_setCmdPos( uc_x, uc_y, en_dir );

}

void MAP_showLog( void )
{
	int16_t	x,y;
	uint8_t	c_data;
	
	/* ---------- */
	/*  通常迷路  */
	/* ---------- */
	printf("\n\r  /* ---------- */   ");
	printf("\n\r  /*  通常迷路  */   ");
	printf("\n\r  /* ---------- */   ");

	printf("\n\r     ");
	for( x=0; x<MAP_X_SIZE; x++){
		printf("._");
	}
	printf(".\n\r");
	
	for( y=MAP_Y_SIZE-1; y>-1; y-- ){
		
		printf("   %2d",y);
		for( x=0; x<MAP_X_SIZE; x++){
			c_data = (uint8_t)g_SysMap[y][x];
			if ( ( c_data & 0x08 ) == 0 ){
				printf(".");
			}
			else{
				printf("|");
			}
			if ( ( c_data & 0x04 ) == 0 ){
				printf(" ");
			}
			else{
				printf("_");
			}
		}
		printf("|   ");
		
		for( x=0; x<MAP_X_SIZE; x++ ){
			c_data = g_SysMap[y][x];
			c_data = c_data >> 4;
			printf("%x", c_data);
		}
		
		printf("\n\r");
	}
	
	printf("     ");
	for( x=0; x<MAP_X_SIZE; x++){
		printf("%2d",x%10);
	}
	printf("\n\r");

}

void MAP_showcountLog(void)
{
	int16_t	x, y;
	uint16_t	c_data;

	/* ---------- */
	/*  等高線迷路  */
	/* ---------- */
	printf("\n\r  /* ------------ */   ");
	printf("\n\r  /*  等高線迷路  */   ");
	printf("\n\r  /* ------------ */   ");

	printf("\n\r");
	for (y = MAP_Y_SIZE - 1; y > -1; y--) {
		for (x = 0; x < MAP_X_SIZE; x++) {
			c_data = (uint16_t)us_Cmap[y][x];
			printf("%3d ", c_data);
		}
		printf("\n\r");
	}
}

void MAP_clearMap( void )
{
	uint16_t	x, y;
	uint8_t	uc_data;

	/* すべてのマップデータを未探索状態にする */
	for ( y = 0; y < MAP_Y_SIZE; y++){
		for( x = 0; x < MAP_X_SIZE; x++){
			uc_data = 0x00;
			if ( ( x == 0) && ( y == 0 ) ) uc_data = 0xfe;
			else if ( ( x == 1 ) && ( y == 0 ) ) uc_data = 0xcc;
			else if ( ( x == (MAP_X_SIZE-1) ) && ( y == 0 ) ) uc_data = 0x66;
			else if ( ( x == 0 ) && ( y == (MAP_Y_SIZE-1) ) ) uc_data = 0x99;
			else if ( ( x == (MAP_X_SIZE-1) ) && ( y == (MAP_Y_SIZE-1) ) ) uc_data = 0x33;
			else if ( x == 0 ) uc_data = 0x88;
			else if ( x == (MAP_X_SIZE-1) ) uc_data = 0x22;
			else if ( y == 0 ) uc_data = 0x44;
			else if ( y == (MAP_Y_SIZE-1) ) uc_data = 0x11;
			g_SysMap[y][x] = uc_data;
		}
	}

}

uint8_t MAP_getWallData( void )
{
	uint8_t	 uc_wall;

//	LED_offAll();			// debug

	// センサ情報から壁情報作成
	uc_wall = 0;
	if( TRUE == DIST_isWall_FRONT() ){
		uc_wall = uc_wall | 0x11;
//		LED_on(LED3);			// debug
//		LED_on(LED2);			// debug
	}
	if( TRUE == DIST_isWall_L_SIDE() ){
//		LED_on(LED0);			// debug
		uc_wall = uc_wall | 0x88;
	}
	if( TRUE == DIST_isWall_R_SIDE() ){
//		LED_on(LED1);			// debug
		uc_wall = uc_wall | 0x22;
	}
	if(DIST_getNowVal(DIST_SEN_L_SIDE)>L_SIDE_REF){
		NearWall = 1;
	}
	if(DIST_getNowVal(DIST_SEN_R_SIDE)>R_SIDE_REF){
		NearWall = 1;
	}

	// マウスの進行方向にあわせてセンサデータを移動し壁データとする
	if		( en_Head == EAST ){
		uc_wall = uc_wall >> 3;
	}
	else if ( en_Head == SOUTH ){
		uc_wall = uc_wall >> 2;
	}
	else if ( en_Head == WEST ){
		uc_wall = uc_wall >> 1;
	}

	//	探索済みフラグを立てる
	return ( uc_wall | 0xf0 );
}

void MAP_makeMapData( void )
{
	uint8_t uc_wall;

	//	走行時の壁情報を迷路情報に書込
	if ( ( mx == 0 ) && ( my == 0 ) ){
		uc_wall = 0xfe;
	}
	else{
		uc_wall = MAP_getWallData();
	}
	g_SysMap[my][mx] = uc_wall;

	//	隣の区間のＭＡＰデータも更新する
	if ( mx != (MAP_X_SIZE-1) ){
		g_SysMap[my][mx+1] = ( g_SysMap[my][mx+1] & 0x77 ) | 0x80 | ( ( uc_wall << 2 ) & 0x08 );
	}
	if ( mx !=  0 ){
		g_SysMap[my][mx-1] = ( g_SysMap[my][mx-1] & 0xdd ) | 0x20 | ( ( uc_wall >> 2 ) & 0x02 );
	}
	if ( my != (MAP_Y_SIZE-1) ){
		g_SysMap[my+1][mx] = ( g_SysMap[my+1][mx] & 0xbb ) | 0x40 | ( ( uc_wall << 2 ) & 0x04 );
	}
	if ( my !=  0 ){
		g_SysMap[my-1][mx] = ( g_SysMap[my-1][mx] & 0xee ) | 0x10 | ( ( uc_wall >> 2 ) & 0x01 );
	}

}

uint8_t MAP_get_refPos_WallData( void )
{
	uint8_t	 uc_wall;

	// センサ情報から壁情報作成
	uc_wall = 0;
	if( TRUE == DIST_isWall_FRONT() ){
		uc_wall = uc_wall | 0x11;
	}
	MOT_turn(MOT_R90);
	if( TRUE == DIST_isWall_FRONT() ){
		uc_wall = uc_wall | 0x22;
	}
	MOT_turn(MOT_R90);
	if( TRUE == DIST_isWall_FRONT() ){
		uc_wall = uc_wall | 0x44;
	}
	MOT_turn(MOT_R90);
	if( TRUE == DIST_isWall_FRONT() ){
		uc_wall = uc_wall | 0x88;
	}
	MOT_turn(MOT_R90);

	// マウスの進行方向にあわせてセンサデータを移動し壁データとする
	if		( en_Head == EAST ){
		uc_wall = uc_wall >> 3;
	}
	else if ( en_Head == SOUTH ){
		uc_wall = uc_wall >> 2;
	}
	else if ( en_Head == WEST ){
		uc_wall = uc_wall >> 1;
	}

	//	探索済みフラグを立てる
	return ( uc_wall | 0xf0 );
}

void MAP_make_refPos_MapData(enMAP_HEAD_DIR en_head)
{
	uint8_t uc_wall;

	//	走行時の壁情報を迷路情報に書込	
	uc_wall = MAP_get_refPos_WallData();
	
	g_SysMap[my][mx] = uc_wall;

	//	隣の区間のＭＡＰデータも更新する
	if ( mx != (MAP_X_SIZE-1) ){
		g_SysMap[my][mx+1] = ( g_SysMap[my][mx+1] & 0x77 ) | 0x80 | ( ( uc_wall << 2 ) & 0x08 );
	}
	if ( mx !=  0 ){
		g_SysMap[my][mx-1] = ( g_SysMap[my][mx-1] & 0xdd ) | 0x20 | ( ( uc_wall >> 2 ) & 0x02 );
	}
	if ( my != (MAP_Y_SIZE-1) ){
		g_SysMap[my+1][mx] = ( g_SysMap[my+1][mx] & 0xbb ) | 0x40 | ( ( uc_wall << 2 ) & 0x04 );
	}
	if ( my !=  0 ){
		g_SysMap[my-1][mx] = ( g_SysMap[my-1][mx] & 0xee ) | 0x10 | ( ( uc_wall >> 2 ) & 0x01 );
	}

}

/*
void  MAP_makeContourMap( //旧型の軽量化コードただし、実際には使えない
	uint8_t uc_goalX, 			///< [in] ゴールX座標
	uint8_t uc_goalY, 			///< [in] ゴールY座標
	enMAP_ACT_MODE	en_type		///< [in] 計算方法（まだ未使用）
){
	uint16_t		x, y, i;		// ループ変数
	uint16_t		uc_dase;		// 基準値
	uint16_t		uc_new;			// 新値
	uint16_t		uc_level;		// 等高線
	uint8_t		uc_wallData;	// 壁情報

	en_type = en_type;		// コンパイルワーニング回避（いずれ削除）

	// 等高線マップを初期化する 
	for ( i = 0; i < MAP_SMAP_MAX_VAL; i++ ){
		us_Cmap[ i / MAP_Y_SIZE][ i & (MAP_X_SIZE-1) ] = MAP_SMAP_MAX_VAL - 1;
	}
	// 目標地点の等高線を0に設定 
	us_Cmap[uc_goalY][uc_goalX] = 0;
	if (GOAL_SIZE == 4) {
		us_Cmap[uc_goalY + 1][uc_goalX] = 0;
		us_Cmap[uc_goalY][uc_goalX + 1] = 0;
		us_Cmap[uc_goalY + 1][uc_goalX + 1] = 0;
	}
	else if (GOAL_SIZE == 9){
		us_Cmap[uc_goalY+1][uc_goalX] = 0;
		us_Cmap[uc_goalY][uc_goalX+1] = 0;
		us_Cmap[uc_goalY+1][uc_goalX+1] = 0;
		us_Cmap[uc_goalY+2][uc_goalX] = 0;
		us_Cmap[uc_goalY+2][uc_goalX+1] = 0;
		us_Cmap[uc_goalY][uc_goalX+2] = 0;
		us_Cmap[uc_goalY+1][uc_goalX+2] = 0;
		us_Cmap[uc_goalY+2][uc_goalX+2] = 0;
	}

	if (mx > uc_max_x)uc_max_x = mx;
	if (my > uc_max_y)uc_max_y = my;

	// 等高線マップを作成 
	uc_dase = 0;
	do{
		uc_level = 0;
		uc_new = uc_dase + 1;
		for ( y = 0; y < MAP_Y_SIZE; y++ ){
			if (uc_max_y+1 < y) break;
			for ( x = 0; x < MAP_X_SIZE; x++ ){
				if (uc_max_x+1 < x) break;
				if ( us_Cmap[y][x] == uc_dase ){
					uc_wallData = g_SysMap[y][x];
					// 探索走行 
					if( SEARCH == en_type ){
						if ( ( ( uc_wallData & 0x01 ) == 0x00 ) && ( y != (MAP_Y_SIZE-1) ) ){
							if ( us_Cmap[y+1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y+1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x02 ) == 0x00 ) && ( x != (MAP_X_SIZE-1) ) ){
							if ( us_Cmap[y][x+1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x+1] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x04 ) == 0x00 ) && ( y != 0 ) ){
							if ( us_Cmap[y-1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y-1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x08 ) == 0x00 ) && ( x != 0 ) ){
							if ( us_Cmap[y][x-1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x-1] = uc_new;
								uc_level++;
							}
						}
					}
					// 最短走行 
					else{
						if ( ( ( uc_wallData & 0x11 ) == 0x10 ) && ( y != (MAP_Y_SIZE-1) ) ){
							if ( us_Cmap[y+1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y+1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x22 ) == 0x20 ) && ( x != (MAP_X_SIZE-1) ) ){
							if ( us_Cmap[y][x+1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x+1] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x44 ) == 0x40 ) && ( y != 0 ) ){
							if ( us_Cmap[y-1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y-1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x88 ) == 0x80 ) && ( x != 0 ) ){
							if ( us_Cmap[y][x-1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x-1] = uc_new;
								uc_level++;
							}
						}
					}
					if ((x == mx)&&(y == my))break;
				}
			}
			if ((x == mx)&&(y == my))break;
		}
		if ((x == mx)&&(y == my))break;
		uc_dase = uc_dase + 1;
	}
	while( uc_level != 0 );
	
}
*/
void setStep(const int8_t x, const int8_t y, const uint16_t step) {
	/* (x, y) がフィールド内か確認 */
	if (x < 0 || y < 0 || x >= MAP_X_SIZE || y >= MAP_Y_SIZE) {
		printf( "referred to out of field\r\n");
		return;
	}
	us_Cmap[y][x] = step;
}

void  MAP_makeContourMap_queue(//queue
	uint8_t uc_goalX, 			///< [in] ゴールX座標
	uint8_t uc_goalY, 			///< [in] ゴールY座標
	enMAP_ACT_MODE	en_type		///< [in] 計算方法（まだ未使用）
) {
	uint16_t		x, y, i;		// ループ変数
	uint16_t		uc_dase;		// 基準値
	uint16_t		uc_new;			// 新値
	uint16_t		uc_level;		// 等高線
	uint8_t		uc_wallData;	// 壁情報

	stPOSITION		st_pos;

	en_type = en_type;		// コンパイルワーニング回避（いずれ削除）

	queue_t queue;
	queue_t* pQueue = &queue;

	InitQueue(pQueue);

	/* 等高線マップを初期化する */
	for (i = 0; i < MAP_SMAP_MAX_VAL; i++) {
		us_Cmap[i / MAP_Y_SIZE][i & (MAP_X_SIZE - 1)] = MAP_SMAP_MAX_VAL - 1;
	}

//	std::queue<stPOSITION> q;
//	QueryPerformanceCounter(&start);

	/* 目標地点の等高線を0に設定 */
	setStep(uc_goalX, uc_goalY, 0);
	st_pos.x = uc_goalX;
	st_pos.y = uc_goalY;
	st_pos.step = 0;

	EnQueue(pQueue,st_pos);

	/* 等高線マップを作成 */
	while (pQueue->flag != EMPTY) {
		const stPOSITION focus = DeQueue(pQueue);
//		q.pop();
		const uint16_t focus_step = focus.step;
		x = focus.x;
		y = focus.y;
		stPOSITION next = focus;
		uc_wallData = g_SysMap[y][x];

		if (((uc_wallData & 0x01) == 0x00) && (y != (MAP_Y_SIZE - 1))) {
			if (us_Cmap[y + 1][x] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y + 1][x] = focus_step + 1;
				next.x = x;
				next.y = y + 1;
				EnQueue(pQueue,next);
			}
		}
		if (((uc_wallData & 0x02) == 0x00) && (x != (MAP_X_SIZE - 1))) {
			if (us_Cmap[y][x + 1] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y][x + 1] = focus_step + 1;
				next.x = x + 1;
				next.y = y;
				EnQueue(pQueue, next);
			}
		}
		if (((uc_wallData & 0x04) == 0x00) && (y != 0)) {
			if (us_Cmap[y - 1][x] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y - 1][x] = focus_step + 1;
				next.x = x;
				next.y = y - 1;
				EnQueue(pQueue, next);
			}
		}
		if (((uc_wallData & 0x08) == 0x00) && (x != 0)) {
			if (us_Cmap[y][x - 1] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y][x - 1] = focus_step + 1;
				next.x = x - 1;
				next.y = y;
				EnQueue(pQueue, next);
			}
		}

	}

}

void  MAP_makeContourMap_run( //初期型のフルマップ生成　最短用に使用
	uint8_t uc_goalX, 			///< [in] ゴールX座標
	uint8_t uc_goalY, 			///< [in] ゴールY座標
	enMAP_ACT_MODE	en_type		///< [in] 計算方法（まだ未使用）
) {
	uint16_t		x, y, i;		// ループ変数
	uint16_t		uc_dase;		// 基準値
	uint16_t		uc_new;			// 新値
	uint16_t		uc_level;		// 等高線
	uint8_t		uc_wallData;	// 壁情報

	en_type = en_type;		// コンパイルワーニング回避（いずれ削除）

	/* 等高線マップを初期化する */
	for ( i = 0; i < MAP_SMAP_MAX_VAL; i++ ){
		us_Cmap[ i / MAP_Y_SIZE][ i & (MAP_X_SIZE-1) ] = MAP_SMAP_MAX_VAL - 1;
	}
	/* 目標地点の等高線を0に設定 */
	us_Cmap[uc_goalY][uc_goalX] = 0;
	if (GOAL_SIZE == 4) {
		us_Cmap[uc_goalY + 1][uc_goalX] = 0;
		us_Cmap[uc_goalY][uc_goalX + 1] = 0;
		us_Cmap[uc_goalY + 1][uc_goalX + 1] = 0;
	}
	else if (GOAL_SIZE == 9){
		us_Cmap[uc_goalY+1][uc_goalX] = 0;
		us_Cmap[uc_goalY][uc_goalX+1] = 0;
		us_Cmap[uc_goalY+1][uc_goalX+1] = 0;
		us_Cmap[uc_goalY+2][uc_goalX] = 0;
		us_Cmap[uc_goalY+2][uc_goalX+1] = 0;
		us_Cmap[uc_goalY][uc_goalX+2] = 0;
		us_Cmap[uc_goalY+1][uc_goalX+2] = 0;
		us_Cmap[uc_goalY+2][uc_goalX+2] = 0;
	}

	/* 等高線マップを作成 */
	uc_dase = 0;
	do{
		uc_level = 0;
		uc_new = uc_dase + 1;
		for ( y = 0; y < MAP_Y_SIZE; y++ ){
			for ( x = 0; x < MAP_X_SIZE; x++ ){
				if ( us_Cmap[y][x] == uc_dase ){
					uc_wallData = g_SysMap[y][x];
					/* 探索走行 */
					if( SEARCH == en_type ){
						if ( ( ( uc_wallData & 0x01 ) == 0x00 ) && ( y != (MAP_Y_SIZE-1) ) ){
							if ( us_Cmap[y+1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y+1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x02 ) == 0x00 ) && ( x != (MAP_X_SIZE-1) ) ){
							if ( us_Cmap[y][x+1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x+1] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x04 ) == 0x00 ) && ( y != 0 ) ){
							if ( us_Cmap[y-1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y-1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x08 ) == 0x00 ) && ( x != 0 ) ){
							if ( us_Cmap[y][x-1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x-1] = uc_new;
								uc_level++;
							}
						}
					}
					/* 最短走行 */
					else{
						if ( ( ( uc_wallData & 0x11 ) == 0x10 ) && ( y != (MAP_Y_SIZE-1) ) ){
							if ( us_Cmap[y+1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y+1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x22 ) == 0x20 ) && ( x != (MAP_X_SIZE-1) ) ){
							if ( us_Cmap[y][x+1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x+1] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x44 ) == 0x40 ) && ( y != 0 ) ){
							if ( us_Cmap[y-1][x] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y-1][x] = uc_new;
								uc_level++;
							}
						}
						if ( ( ( uc_wallData & 0x88 ) == 0x80 ) && ( x != 0 ) ){
							if ( us_Cmap[y][x-1] == MAP_SMAP_MAX_VAL - 1 ){
								us_Cmap[y][x-1] = uc_new;
								uc_level++;
							}
						}
					}
				}
			}
		}
		uc_dase = uc_dase + 1;
	}
	while( uc_level != 0 );
	
}

void MAP_calcMouseDir( 
	enMAP_SEARCH_TYPE	en_calcType,	///< [in] 計算方法
	enMAP_HEAD_DIR* 	p_head			///< [out] 進行方向（戻り値）
){
	uint8_t		uc_wall;				// 壁情報
	uint16_t		us_base;			// 等高線優先度決定値
	uint16_t		us_new;
	enMAP_HEAD_DIR	en_tmpHead;

	/* 方向計算 */
	// 等高線MAP法
	if( CONTOUR_SYSTEM == en_calcType ){
		// 周辺の4区画で一番目的地に近い移動方向を算出する。
		// ただし、移動できる一番近い区間が複数ある場合には、次の順で選択する。
		// ①未探索区間,直進 ②未探索区間,旋回 ③既探索区間,直進 ④既探索区間,旋回
		uc_wall = g_SysMap[my][mx];
		us_base = MAP_SMAP_MAX_PRI_VAL;					// 16[区画]×16[区画]×4[方向]

		/* 4方向を比較 */
		//	北方向の区画の確認
		if ( ( uc_wall & 1 ) == 0 ){
			us_new = us_Cmap[my+1][mx] * 4 + 4;
			if ( ( g_SysMap[my+1][mx] & 0xf0 ) != 0xf0 ) us_new = us_new - 2;
			if ( en_Head == NORTH ) us_new = us_new - 1;
			if ( us_new < us_base ){
				us_base = us_new;
				en_tmpHead = NORTH;
			}
		}
		//	東方向の区画の確認
		if ( ( uc_wall & 2 ) == 0 ){
			us_new = us_Cmap[my][mx+1] * 4 + 4;
			if ( ( g_SysMap[my][mx+1] & 0xf0 ) != 0xf0 ) us_new = us_new - 2;
			if ( en_Head == EAST) us_new = us_new - 1;
			if ( us_new < us_base ){
				us_base = us_new;
				en_tmpHead = EAST;
			}
		}
		//	南方向の区画の確認
		if ( ( uc_wall & 4 ) == 0 ){
			us_new = us_Cmap[my-1][mx] * 4 + 4;
			if ( ( g_SysMap[my-1][mx] & 0xf0 ) != 0xf0) us_new = us_new - 2;
			if ( en_Head == SOUTH ) us_new = us_new - 1;
			if ( us_new < us_base ){
				us_base = us_new;
				en_tmpHead = SOUTH;
			}
		}
		//	西方向の区画の確認
		if ( ( uc_wall & 8 ) == 0 ){
			us_new = us_Cmap[my][mx-1] * 4 + 4;
			if ( ( g_SysMap[my][mx-1] & 0xf0 ) != 0xf0 ) us_new = us_new - 2;
			if ( en_Head == WEST ) us_new = us_new - 1;
			if ( us_new < us_base ){
				us_base = us_new;
				en_tmpHead = WEST;
			}
		}
		
		*p_head = (enMAP_HEAD_DIR)( (en_tmpHead - en_Head) & 3 );		// 移動方向
		
	}
	// 制御方法指定なし
	else{
		*p_head = (enMAP_HEAD_DIR)0;
	}

}

void MAP_refMousePos( 
	enMAP_HEAD_DIR 			en_head			///< [in] 進行方向
){
	switch( en_head ){
		case NORTH:
			my = my + 1;
			break;
		case EAST:
			mx = mx + 1;
			break;
		case SOUTH:
			my = my - 1;
			break;
		case WEST:
			mx = mx - 1;
			break;
		default:
			break;
	}
}

void MAP_moveNextBlock( 
	enMAP_HEAD_DIR 	en_head,		///< [in] 相対進行方向（マウス進行方向を北としている）
	bool*			p_type			///< [in] FALSE: １区間前進状態、TURE:半区間前進状態
){
	*p_type = TRUE;
	f_MoveBackDist = 0;				// 移動距離を加算値クリア
	
	/* 動作 */
	switch( en_head ){

		/* そのまま前進 */
		case NORTH:
			*p_type = FALSE;
			MOT_goBlock_Const( 1 );				// 1区画前進
			break;
		// 右に旋回する
		case EAST:
			MOT_goBlock_FinSpeed( 0.5, 0 );		// 半区画前進
			MOT_turn(MOT_R90);					// 右90度旋回
			break;
		// 左に旋回する
		case WEST:
			MOT_goBlock_FinSpeed( 0.5, 0 );		// 半区画前進
			MOT_turn(MOT_L90);					// 右90度旋回
			break;
		// 反転して戻る
		case SOUTH:
			MOT_goBlock_FinSpeed( 0.5, 0 );		// 半区画前進
			MOT_turn(MOT_R180);					// 右180度旋回
			
			/* 壁当て姿勢制御（後ろに壁があったらバック＋移動距離を加算する） */
			if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 北を向いていて北に壁がある
				( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 東を向いていて東に壁がある
				( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 南を向いていて南に壁がある
				( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) ) 			// 西を向いていて西に壁がある
			){
				MOT_goHitBackWall();					// バックする
				f_MoveBackDist = MOVE_BACK_DIST;		// バックした分の移動距離[区画]を加算
			}
			break;
		default:
			break;
	}
	
	/* 前進中にパワーリリース機能が働いてレジュームしなければならない */
/*	if( ( TRUE == DCMC_isPowerRelease() ) && ( en_head == NORTH ) ){
		
		MOT_goBack_Const( MOT_BACK_POLE );					// １つ前の柱まで後退
		MAP_makeMapData();									// 壁データから迷路データを作成			← ここでデータ作成をミスっている
		MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);			// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
		MAP_moveNextBlock(en_head, p_type);					// もう１度呼び出し（次の区画へ移動）
	}
	else{*/
		/* 進行方向更新 */
		en_Head = (enMAP_HEAD_DIR)( (en_Head + en_head) & (MAP_HEAD_DIR_MAX-1) );
//	}
}

void MAP_moveNextBlock_Sla( 
	enMAP_HEAD_DIR 	en_head,		///< [in] 相対進行方向（マウス進行方向を北としている
	bool*			p_type,			///< [in] FALSE: １区間前進状態、TURE:半区間前進状態
	bool			bl_resume		///< [in] FALSE: レジューム動作ではない、TURE:レジューム動作
){
	*p_type = FALSE;
	f_MoveBackDist = 0;				// 移動距離を加算値クリア
	
	/* 動作 */
	switch( en_head ){

		// そのまま前進
		case NORTH:
			
			/* レジューム動作ではない */
			if( bl_resume == FALSE ){
		
				MOT_goBlock_Const( 1 );					// 1区画前進
//				uc_SlaCnt = 0;							// スラロームしていない
			}
			/* レジューム動作 */
			else{
				MOT_goBlock_FinSpeed( 1.0f, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
//				uc_SlaCnt = 0;									// スラロームしていない
			}
			break;

		// 右にスラロームする
		case EAST:
			if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 北を向いていて東に壁がある
					( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 東を向いていて南に壁がある
					( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 南を向いていて西に壁がある
					( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) ) 			// 西を向いていて北に壁がある
				){
				uc_DistControl = 0.01;
				}
			else{
				uc_DistControl = 0;
			}
			if( uc_SlaCnt < SLA_Count ){
				MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );	// 右スラローム
				uc_SlaCnt++;
			}
			else{
				/* 壁当て姿勢制御（左に壁があったらバック＋移動距離を加算する） */
				if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 北を向いていて西に壁がある
					( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 東を向いていて北に壁がある
					( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 南を向いていて東に壁がある
					( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) ) 			// 西を向いていて南に壁がある
				){
					MOT_goBlock_FinSpeed( 0.5, 0 );			// 半区画前進
					MOT_turn(MOT_R90);						// 右90度旋回
					uc_SlaCnt = 0;
					MOT_goHitBackWall();					// バックする
					f_MoveBackDist = MOVE_BACK_DIST;		// バックした分の移動距離[区画]を加算
					*p_type = TRUE;							// 次は半区間（＋バック）分進める
				}
				else{
					MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );	// 右スラローム
					uc_SlaCnt++;
				}
			}
			break;

		// 左にスラロームする
		case WEST:
			if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 北を向いていて西に壁がある
					( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 東を向いていて北に壁がある
					( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 南を向いていて東に壁がある
					( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) ) 			// 西を向いていて南に壁がある
				){
				uc_DistControl = 0.01;
				}
			else{
				uc_DistControl = 0;
			}
			if( uc_SlaCnt < SLA_Count ){
				MOT_goSla( MOT_L90S, PARAM_getSra( SLA_90 ) );	// 左スラローム
				uc_SlaCnt++;
			}
			else{
				/* 壁当て姿勢制御（後ろに壁があったらバック＋移動距離を加算する） */
				if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 北を向いていて東に壁がある
					( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 東を向いていて南に壁がある
					( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 南を向いていて西に壁がある
					( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) ) 			// 西を向いていて北に壁がある
				){
					MOT_goBlock_FinSpeed( 0.5, 0 );		// 半区画前進
					MOT_turn(MOT_L90);					// 右90度旋回
					uc_SlaCnt = 0;
					MOT_goHitBackWall();					// バックする
					f_MoveBackDist = MOVE_BACK_DIST;		// バックした分の移動距離[区画]を加算
					*p_type = TRUE;							// 次は半区間（＋バック）分進める
				}
				else{
					MOT_goSla( MOT_L90S, PARAM_getSra( SLA_90 ) );	// 左スラローム
					uc_SlaCnt++;
				}
			}
			break;

		// 反転して戻る
		case SOUTH:
			MOT_goBlock_FinSpeed( 0.5, 0 );			// 半区画前進
			MOT_turn(MOT_R180);									// 右180度旋回
			uc_SlaCnt = 0;
			
			/* 壁当て姿勢制御（後ろに壁があったらバック＋移動距離を加算する） */
			if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 北を向いていて北に壁がある
				( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 東を向いていて東に壁がある
				( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 南を向いていて南に壁がある
				( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) ) 			// 西を向いていて西に壁がある
			){
				MOT_goHitBackWall();					// バックする
				f_MoveBackDist = MOVE_BACK_DIST;		// バックした分の移動距離[区画]を加算
			}
			*p_type = TRUE;								// 次は半区間＋バック分進める
			break;
			
		default:
			break;
	}
	
	/* 前進中にパワーリリース機能が働いてレジュームしなければならない */
/*	if( ( TRUE == DCMC_isPowerRelease() ) && ( en_head == NORTH ) ){
		
		MOT_goBack_Const( MOT_BACK_POLE );					// １つ前の柱まで後退
		MAP_makeMapData();									// 壁データから迷路データを作成			← ここでデータ作成をミスっている
		MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);			// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
		MAP_moveNextBlock_Sla(en_head, p_type, TRUE );		// もう１度呼び出し（次の区画へ移動）
	}
	else{*/
		/* 進行方向更新 */
		en_Head = (enMAP_HEAD_DIR)( (en_Head + en_head) & (MAP_HEAD_DIR_MAX-1) );
//	}
}

void MAP_moveNextBlock_frontmiss( 
	enMAP_HEAD_DIR 	en_head,		///< [in] 相対進行方向（マウス進行方向を北としている）
	bool*			p_type			///< [in] FALSE: １区間前進状態、TURE:半区間前進状態
){
	*p_type = FALSE;
	f_MoveBackDist = 0;				// 移動距離を加算値クリア
	
	/* 動作 */
	switch( en_head ){

		/* そのまま前進 */
		case NORTH:
			MOT_goBlock_FinSpeed( 0.5, SEARCH_SPEED );				// 1区画前進
			break;
		// 右に旋回する
		case EAST:
			MOT_turn(MOT_R90);					// 右90度旋回
			MOT_goBlock_FinSpeed( 0.5, SEARCH_SPEED );		// 半区画前進
			break;
		// 左に旋回する
		case WEST:
			MOT_turn(MOT_L90);					// 右90度旋回
			MOT_goBlock_FinSpeed( 0.5, SEARCH_SPEED );		// 半区画前進
			break;
		// 反転して戻る
		case SOUTH:
			MOT_turn(MOT_R180);					// 右180度旋回
			MOT_goBlock_FinSpeed( 0.5, SEARCH_SPEED );		// 半区画前進
			break;
		default:
			break;
	}

	en_Head = (enMAP_HEAD_DIR)( (en_Head + en_head) & (MAP_HEAD_DIR_MAX-1) );

}

void MAP_actGoal( void )
{	
	MOT_goBlock_FinSpeed( 0.5, 0 );			// 半区画前進
	LL_mDelay(500);
	MOT_turn(MOT_R180);										// 右180度旋回
	LL_mDelay(500);
	
//	MAP_SaveMapData();						// 迷路情報のバックアップ
	log_flag_off();
	MAP_actGoalLED();
	
	en_Head = (enMAP_HEAD_DIR)( (en_Head + 2) & (MAP_HEAD_DIR_MAX-1) );			//	進行方向更新

}

void MAP_actGoalLED( void )
{
	int i;
	for(i = 0;i<2;i++)
	{
		SetLED(0x02);
		LL_mDelay(100);
		SetLED(0x04);
		LL_mDelay(100);
		SetLED(0x08);
		LL_mDelay(100);
		SetLED(0x04);
		LL_mDelay(100);
		SetLED(0x02);
		LL_mDelay(100);
	}
	LL_mDelay(100);
	Map_Write();
	SetLED(0x00);
}

void MAP_Goalsize(int size)
{
	GOAL_SIZE= size;
	if (size == 4) {
		uc_max_x = uc_max_x + 1;
		uc_max_y = uc_max_y + 1;
	}
	else if (size == 9) {
		uc_max_x = uc_max_x + 2;
		uc_max_y = uc_max_y + 2;
	}
}

void  MAP_makeReturnContourMap(uint8_t uc_staX,uint8_t uc_staY) 
{
	uint16_t		x, y, i;		// ループ変数
	uint16_t		uc_dase;		// 基準値
	uint16_t		uc_new;			// 新値
	uint16_t		uc_level;		// 等高線
	uint8_t		uc_wallData;	// 壁情報

	stPOSITION		st_pos;
	queue_t queue;
	queue_t* pQueue = &queue;

	InitQueue(pQueue);

	/* 等高線マップを初期化する */
	for (i = 0; i < MAP_SMAP_MAX_VAL; i++) {
		us_Cmap[i / MAP_Y_SIZE][i & (MAP_X_SIZE - 1)] = MAP_SMAP_MAX_VAL - 1;
	}
	/* 目標地点の等高線を0に設定 */
//	us_Cmap[0][0] = 0;
	setStep(0, 0, 0);
	st_pos.x = 0;
	st_pos.y = 0;
	st_pos.step = 0;

	EnQueue(pQueue,st_pos);

	/* 等高線マップを作成 */
	while (pQueue->flag != EMPTY) {
		const stPOSITION focus = DeQueue(pQueue);
//		q.pop();
		const uint16_t focus_step = focus.step;
		x = focus.x;
		y = focus.y;
		stPOSITION next = focus;
		uc_wallData = g_SysMap[y][x];

		if (((uc_wallData & 0x01) == 0x00) && (y != (MAP_Y_SIZE - 1))) {
			if (us_Cmap[y + 1][x] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y + 1][x] = focus_step + 1;
				next.x = x;
				next.y = y + 1;
				EnQueue(pQueue,next);
			}
		}
		if (((uc_wallData & 0x02) == 0x00) && (x != (MAP_X_SIZE - 1))) {
			if (us_Cmap[y][x + 1] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y][x + 1] = focus_step + 1;
				next.x = x + 1;
				next.y = y;
				EnQueue(pQueue, next);
			}
		}
		if (((uc_wallData & 0x04) == 0x00) && (y != 0)) {
			if (us_Cmap[y - 1][x] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y - 1][x] = focus_step + 1;
				next.x = x;
				next.y = y - 1;
				EnQueue(pQueue, next);
			}
		}
		if (((uc_wallData & 0x08) == 0x00) && (x != 0)) {
			if (us_Cmap[y][x - 1] > focus_step + 1) {
				next.step = focus_step + 1;
				us_Cmap[y][x - 1] = focus_step + 1;
				next.x = x - 1;
				next.y = y;
				EnQueue(pQueue, next);
			}
		}

	}

}

bool MAP_KnownAcc(void) {

	bool	bl_acc = FALSE;
	switch (en_Head) {
	case NORTH:
		if ((g_SysMap[my + 1][mx] & 0xf1) == 0xf0) {
			bl_acc = TRUE;
		}

		break;

	case EAST:
		if ((g_SysMap[my][mx + 1] & 0xf2) == 0xf0) {
			bl_acc = TRUE;
		}
		break;

	case SOUTH:
		if ((g_SysMap[my - 1][mx] & 0xf4) == 0xf0) {
			bl_acc = TRUE;
		}
		break;

	case WEST:
		if ((g_SysMap[my][mx - 1] & 0xf8) == 0xf0) {
			bl_acc = TRUE;
		}
		break;

	default:
		break;
	}
	return	bl_acc;
}

void MAP_moveNextBlock_acc(enMAP_HEAD_DIR en_head, bool* p_type)
{
	*p_type = FALSE;
	f_MoveBackDist = 0;

	/* 動作 */
	switch (en_head) {

		/* そのまま前進 */
	case NORTH:
//		*p_type = FALSE;
//		LED = LED6;
		if (MAP_KnownAcc() == FALSE) {					// 次に進む区画が未探索のとき
			if (st_Known.bl_known == TRUE){
				if (st_Known.uc_strCnt < 2) {
					MOT_goBlock_Const(1);					// 1区画の場合は等速のまま
				}
				else {
					MOT_setTrgtSpeed(MAP_KNOWN_ACC_SPEED);									// 既知区間加速するときの目標速度	
					MOT_goBlock_FinSpeed((float)(st_Known.uc_strCnt), SEARCH_SPEED);				// n区画前進
					MOT_setTrgtSpeed(SEARCH_SPEED);										// 目標速度をデフォルト値に戻す
				}
			}
			MOT_goBlock_Const(1);	////////////////////
			st_Known.uc_strCnt = 0;
			st_Known.bl_known = FALSE;

		}
		else {

			st_Known.uc_strCnt++;			// 移動区画の加算
			st_Known.bl_known = TRUE;
		}

		break;

		/* 右に旋回する */
	case EAST:
//		LED = LED8;
		if (st_Known.bl_known == TRUE) {		// 直線分を消化
			if (st_Known.uc_strCnt < 2) {
				MOT_goBlock_Const(1);					// 1区画の場合は等速のまま
			}
			else {
//				LED = LED_ALL_ON;
				MOT_setTrgtSpeed(MAP_KNOWN_ACC_SPEED);									// 既知区間加速するときの目標速度	
				MOT_goBlock_FinSpeed((float)(st_Known.uc_strCnt), SEARCH_SPEED);				// n区画前進
				MOT_setTrgtSpeed(SEARCH_SPEED);										// 目標速度をデフォルト値に戻す
//				LED = LED_ALL_OFF;
			}
			st_Known.uc_strCnt = 0;		/////////////////////////////////////////
			st_Known.bl_known = FALSE;
		}

		if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 北を向いていて東に壁がある
					( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 東を向いていて南に壁がある
					( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 南を向いていて西に壁がある
					( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) ) 			// 西を向いていて北に壁がある
			){
			uc_DistControl = 0.02;
			}
		else{
			uc_DistControl = 0;
		}
		if( uc_SlaCnt < SLA_Count ){
				MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );	// 右スラローム
				uc_SlaCnt++;
			}
			else{
				if(WallHitFlag == 0){
					f_MoveBackDist = 0;
					/* 壁当て姿勢制御（左に壁があったらバック＋移動距離を加算する） */
					if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 北を向いていて西に壁がある
						( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 東を向いていて北に壁がある
						( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 南を向いていて東に壁がある
						( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) ) 			// 西を向いていて南に壁がある
					){
						MOT_goBlock_FinSpeed( 0.5, 0 );			// 半区画前進
						MOT_turn(MOT_R90);						// 右90度旋回
						uc_SlaCnt = 0;
						MOT_goHitBackWall();					// バックする
						f_MoveBackDist = MOVE_BACK_DIST;		// バックした分の移動距離[区画]を加算
						*p_type = TRUE;							// 次は半区間（＋バック）分進める
					}
					else{
						MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );	// 右スラローム
						uc_SlaCnt++;
					}
				}
				else{
					/* 前壁制御（正面に壁があったら前壁補正を実行） */
					if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 北を向いていて北に壁がある
						( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 東を向いていて東に壁がある
						( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 南を向いていて南に壁がある
						( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) ) 			// 西を向いていて西に壁がある
					){
						MOT_goBlock_FinSpeed( 0.5, 0 );			// 半区画前進
						LL_mDelay(100);
						DIST_Front_Wall_correction();
						MOT_turn(MOT_R90);						// 右90度旋回
						uc_SlaCnt = 0;
						*p_type = TRUE;								// 次は半区間＋バック分進める
					}
					else{
						MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );	// 右スラローム
						uc_SlaCnt++;
					}
				}
			}
		break;

		/* 左に旋回する */
	case WEST:
		if (st_Known.bl_known == TRUE) {		// 直線分を消化
			if (st_Known.uc_strCnt < 2) {
				MOT_goBlock_Const(1);					// 1区画の場合は等速のまま
			}
			else {
//				LED = LED_ALL_ON;
				MOT_setTrgtSpeed(MAP_KNOWN_ACC_SPEED);									// 既知区間加速するときの目標速度	
				MOT_goBlock_FinSpeed((float)(st_Known.uc_strCnt), SEARCH_SPEED);		// n区画前進
				MOT_setTrgtSpeed(SEARCH_SPEED);										// 目標速度をデフォルト値に戻す
//				LED = LED_ALL_OFF;
			}
			st_Known.uc_strCnt = 0;			//////////////////////////////////////
			st_Known.bl_known = FALSE;
		}

		if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 北を向いていて西に壁がある
					( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 東を向いていて北に壁がある
					( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 南を向いていて東に壁がある
					( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) ) 			// 西を向いていて南に壁がある
			){
			uc_DistControl = 0.02;
			}
		else{
			uc_DistControl = 0;
		}

		if( uc_SlaCnt < SLA_Count ){
				MOT_goSla( MOT_L90S, PARAM_getSra( SLA_90 ) );	// 左スラローム
				uc_SlaCnt++;
			}
			else{
				if(WallHitFlag == 0){
					f_MoveBackDist = 0;
					/* 壁当て姿勢制御（後ろに壁があったらバック＋移動距離を加算する） */
					if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 北を向いていて東に壁がある
						( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 東を向いていて南に壁がある
						( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) )  ||		// 南を向いていて西に壁がある
						( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) ) 			// 西を向いていて北に壁がある
					){
						MOT_goBlock_FinSpeed( 0.5, 0 );		// 半区画前進
						MOT_turn(MOT_L90);					// 右90度旋回
						uc_SlaCnt = 0;
						MOT_goHitBackWall();					// バックする
						f_MoveBackDist = MOVE_BACK_DIST;		// バックした分の移動距離[区画]を加算
						*p_type = TRUE;							// 次は半区間（＋バック）分進める
					}
					else{
						MOT_goSla( MOT_L90S, PARAM_getSra( SLA_90 ) );	// 左スラローム
						uc_SlaCnt++;
					}
				}
				else{
					/* 前壁制御（正面に壁があったら前壁補正を実行） */
					if( ( ( en_Head == NORTH ) && ( ( g_SysMap[my][mx] & 0x01 ) != 0 ) )  ||		// 北を向いていて北に壁がある
						( ( en_Head == EAST  ) && ( ( g_SysMap[my][mx] & 0x02 ) != 0 ) )  ||		// 東を向いていて東に壁がある
						( ( en_Head == SOUTH ) && ( ( g_SysMap[my][mx] & 0x04 ) != 0 ) )  ||		// 南を向いていて南に壁がある
						( ( en_Head == WEST  ) && ( ( g_SysMap[my][mx] & 0x08 ) != 0 ) ) 			// 西を向いていて西に壁がある
					){
						MOT_goBlock_FinSpeed( 0.5, 0 );			// 半区画前進
						LL_mDelay(100);
						DIST_Front_Wall_correction();
						MOT_turn(MOT_L90);						// 右90度旋回
						uc_SlaCnt = 0;
						*p_type = TRUE;								// 次は半区間＋バック分進める
					}
					else{
						MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );	// 右スラローム
						uc_SlaCnt++;
					}
				}
			}
		break;

		/* 反転して戻る */
	case SOUTH:
//		LED = LED_ALL_ON;
		MOT_goBlock_FinSpeed(0.5, 0);			// 半区画前進
		if(WallHitFlag == 0){
			MOT_turn(MOT_R180);									// 右180度旋回
			uc_SlaCnt = 0;

			/* 壁当て姿勢制御（後ろに壁があったらバック＋移動距離を加算する） */
			if (((en_Head == NORTH) && ((g_SysMap[my][mx] & 0x01) != 0)) ||		// 北を向いていて北に壁がある
				((en_Head == EAST) && ((g_SysMap[my][mx] & 0x02) != 0)) ||		// 東を向いていて東に壁がある
				((en_Head == SOUTH) && ((g_SysMap[my][mx] & 0x04) != 0)) ||		// 南を向いていて南に壁がある
				((en_Head == WEST) && ((g_SysMap[my][mx] & 0x08) != 0)) 			// 西を向いていて西に壁がある
				) {
				MOT_goHitBackWall();					// バックする
				f_MoveBackDist = MOVE_BACK_DIST;	// バックした分の移動距離[区画]を加算
			}
			*p_type = TRUE;								// 次は半区間＋バック分進める
		}
		else{
			uc_SlaCnt = 0;
			/* 前壁姿勢制御 */
			if (((en_Head == NORTH) && ((g_SysMap[my][mx] & 0x01) != 0)) ||		// 北を向いていて北に壁がある
				((en_Head == EAST) && ((g_SysMap[my][mx] & 0x02) != 0)) ||		// 東を向いていて東に壁がある
				((en_Head == SOUTH) && ((g_SysMap[my][mx] & 0x04) != 0)) ||		// 南を向いていて南に壁がある
				((en_Head == WEST) && ((g_SysMap[my][mx] & 0x08) != 0)) 			// 西を向いていて西に壁がある
				) {
					LL_mDelay(100);
					DIST_Front_Wall_correction();

					if(((en_Head == NORTH) && ((g_SysMap[my][mx] & 0x02) != 0)) ||		// 北を向いていて東に壁がある
						((en_Head == EAST) && ((g_SysMap[my][mx] & 0x04) != 0)) ||		// 東を向いていて南に壁がある
						((en_Head == SOUTH) && ((g_SysMap[my][mx] & 0x08) != 0)) ||		// 南を向いていて西に壁がある
						((en_Head == WEST) && ((g_SysMap[my][mx] & 0x01) != 0)) 			// 西を向いていて北に壁がある
					){
						MOT_turn(MOT_R90);									// 右90度旋回
						LL_mDelay(100);
						DIST_Front_Wall_correction();
						MOT_turn(MOT_R90);									// 右90度旋回
					}else if(((en_Head == NORTH) && ((g_SysMap[my][mx] & 0x08) != 0)) ||		// 北を向いていて西に壁がある
						((en_Head == EAST) && ((g_SysMap[my][mx] & 0x01) != 0)) ||		// 東を向いていて北に壁がある
						((en_Head == SOUTH) && ((g_SysMap[my][mx] & 0x02) != 0)) ||		// 南を向いていて東に壁がある
						((en_Head == WEST) && ((g_SysMap[my][mx] & 0x04) != 0)) 			// 西を向いていて南に壁がある
					){
						MOT_turn(MOT_L90);									// 右90度旋回
						LL_mDelay(100);
						DIST_Front_Wall_correction();
						MOT_turn(MOT_L90);									// 右90度旋回
					}else{
						MOT_turn(MOT_R180);
					}
					*p_type = TRUE;								// 次は半区間＋バック分進める
			}
			else{
				if(((en_Head == NORTH) && ((g_SysMap[my][mx] & 0x02) != 0)) ||		// 北を向いていて東に壁がある
					((en_Head == EAST) && ((g_SysMap[my][mx] & 0x04) != 0)) ||		// 東を向いていて南に壁がある
					((en_Head == SOUTH) && ((g_SysMap[my][mx] & 0x08) != 0)) ||		// 南を向いていて西に壁がある
					((en_Head == WEST) && ((g_SysMap[my][mx] & 0x01) != 0)) 			// 西を向いていて北に壁がある
				){
					MOT_turn(MOT_R90);									// 右90度旋回
					LL_mDelay(100);
					DIST_Front_Wall_correction();
					MOT_turn(MOT_R90);									// 右90度旋回
				}else if(((en_Head == NORTH) && ((g_SysMap[my][mx] & 0x08) != 0)) ||		// 北を向いていて西に壁がある
					((en_Head == EAST) && ((g_SysMap[my][mx] & 0x01) != 0)) ||		// 東を向いていて北に壁がある
					((en_Head == SOUTH) && ((g_SysMap[my][mx] & 0x02) != 0)) ||		// 南を向いていて東に壁がある
					((en_Head == WEST) && ((g_SysMap[my][mx] & 0x04) != 0)) 			// 西を向いていて南に壁がある
				){
					MOT_turn(MOT_L90);									// 右90度旋回
					LL_mDelay(100);
					DIST_Front_Wall_correction();
					MOT_turn(MOT_L90);									// 右90度旋回
				}else{
					MOT_turn(MOT_R180);
				}
				
				*p_type = TRUE;								// 次は半区間＋バック分進める
			}
		}
		break;

	default:
		break;
	}

	/* 進行方向更新 */
//	en_Head = (enMAP_HEAD_DIR)( (en_Head + en_head) & (MAP_HEAD_DIR_MAX-1) );
	en_Head = (enMAP_HEAD_DIR)(((uint8_t)en_Head + (uint8_t)en_head) & (MAP_HEAD_DIR_MAX - 1));
}

void MAP_searchGoal(
	uint8_t 			uc_trgX, 		///< [in] 目標x座標
	uint8_t 			uc_trgY, 		///< [in] 目標y座標 
	enMAP_ACT_MODE 	en_type, 		///< [in] 探索方法
	enSEARCH_MODE	en_search 		///< [in] 探索方法
){
	enMAP_HEAD_DIR	en_head = NORTH;
	bool		bl_type = TRUE;			// 現在位置、FALSE: １区間前進状態、TURE:半区間前進状態
	enMAP_HEAD_DIR		en_endDir;
	
	uint8_t uc_goalX;
	uint8_t uc_goalY;
	uint8_t uc_staX;
	uint8_t uc_staY;
	
	SearchFlag = TRUE;

	if (en_search == SEARCH_RETURN){
		uc_goalX = uc_trgX;
		uc_goalY = uc_trgY;
		uc_staX = mx;
		uc_staY = my;
//		printf("mx%d,my%d\n", mx, my);
//		MAP_makeContourMap(uc_trgX, uc_trgY, en_type);
		MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
		MAP_searchCmdList(uc_staX, uc_staY, en_Head, uc_goalX, uc_goalX, &en_endDir);
		uc_trgX = Return_X;
		uc_trgY = Return_Y;
//		printf("goalx%d,goaly%d\n", Return_X, Return_Y);
//		MAP_showcountLog();
	}

//	SYS_setDisable( SYS_MODE );				// モード変更禁止

	MOT_setTrgtSpeed(SEARCH_SPEED);		// 目標速度
	MOT_setNowSpeed( 0.0f );
	f_MoveBackDist = 0;
	uc_SlaCnt = 0;
	if(uc_trgX == GOAL_MAP_X && uc_trgY == GOAL_MAP_Y){
		f_MoveBackDist = MOVE_BACK_DIST;
	}
	
	log_flag_on();	//ログ関数スタート（大会時削除）
	
	/* 迷路探索 */
	while(1){
		MAP_refMousePos( en_Head );								// 座標更新
//		MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
		
		/* 超信地旋回探索 */
		if( SEARCH_TURN == en_search ){
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			if( TRUE == bl_type ){
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			MAP_makeMapData();												// 壁データから迷路データを作成			← ここでデータ作成をミスっている
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);						// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
			
			/* 次の区画へ移動 */
			if(( mx == uc_trgX ) && ( my == uc_trgY )){
				MAP_actGoal();										// ゴール時の動作
				break;
			}
			else{
				MAP_moveNextBlock(en_head, &bl_type);				// 次の区画へ移動			← ここで改めてリリースチェック＋壁再度作成＋等高線＋超信地旋回動作
			}
		}
		/* スラローム探索 */
		else if( SEARCH_SLA == en_search ){
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			if( TRUE == bl_type ){
				
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			MAP_makeMapData();		// 壁データから迷路データを作成
			
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);				// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
			
			/* 次の区画へ移動 */
			if(( mx == uc_trgX ) && ( my == uc_trgY )){
				MAP_actGoal();										// ゴール時の動作
				break;
			}
			else{
				MAP_moveNextBlock_Sla(en_head, &bl_type, FALSE );	// 次の区画へ移動						← ここで改めてリリースチェック＋壁再度作成＋等高線＋超信地旋回動作
//				MAP_moveNextBlock_acc(en_head, &bl_type);
			}
		}
		/* 帰還探索 */
		else if (SEARCH_RETURN == en_search) {
			
			if( TRUE == bl_type ){
				
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			MAP_makeMapData();		// 壁データから迷路データを作成
						
			MAP_makeReturnContourMap(uc_staX,uc_staY);
			MAP_searchCmdList(uc_staX, uc_staY, en_Head, uc_goalX, uc_goalX, &en_endDir);
			uc_trgX = Return_X;
			uc_trgY = Return_Y;
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);	
			/* 次の区画へ移動 */
//			if ((us_Cmap[my][mx] == 0)||((g_SysMap[uc_trgY][uc_trgX]&0xf0) == 0xf0)) {
			if ((mx == 0)&&(my == 0)){
				MAP_actGoal();
				break;
			}
//			}
			else {
				MAP_moveNextBlock_Sla(en_head, &bl_type, FALSE);	// 次の区画へ移動			← ここで改めてリリースチェック＋壁再度作成＋等高線＋超信地旋回動作
//				MAP_moveNextBlock_acc(en_head, &bl_type);
			}
//			LED_count(uc_trgY);
		}

		
		/* 途中で制御不能になった */
		if( SYS_isOutOfCtrl() == TRUE ){
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			
			/* 迷路関連を初期化 */
			en_Head		= NORTH;
			mx			= 0;
			my			= 0;
			f_MoveBackDist = 0;
			
			// DCMCは下位モジュールで既にクリアと緊急停止を行っている。
			break;
		}
	}
	SearchFlag = FALSE;
	LL_mDelay(1000);
//	SYS_setEnable( SYS_MODE );				// モード変更有効
}

void MAP_searchGoalKnown(
	uint8_t 			uc_trgX, 		///< [in] 目標x座標
	uint8_t 			uc_trgY, 		///< [in] 目標y座標 
	enMAP_ACT_MODE 	en_type, 		///< [in] 探索方法
	enSEARCH_MODE	en_search 		///< [in] 探索方法
){
	enMAP_HEAD_DIR	en_head = NORTH;
	bool		bl_type = TRUE;			// 現在位置、FALSE: １区間前進状態、TURE:半区間前進状態
	enMAP_HEAD_DIR		en_endDir;
	
	uint8_t uc_goalX;
	uint8_t uc_goalY;
	uint8_t uc_staX;
	uint8_t uc_staY;
	
	SearchFlag = TRUE;

	if (en_search == SEARCH_RETURN){
		uc_goalX = uc_trgX;
		uc_goalY = uc_trgY;
		uc_staX = mx;
		uc_staY = my;
//		printf("mx%d,my%d\n", mx, my);
//		MAP_makeContourMap(uc_trgX, uc_trgY, en_type);
		MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
		MAP_searchCmdList(uc_staX, uc_staY, en_Head, uc_goalX, uc_goalX, &en_endDir);
		uc_trgX = Return_X;
		uc_trgY = Return_Y;
//		printf("goalx%d,goaly%d\n", Return_X, Return_Y);
//		MAP_showcountLog();
	}

//	SYS_setDisable( SYS_MODE );				// モード変更禁止

	MOT_setTrgtSpeed(SEARCH_SPEED);		// 目標速度
	MOT_setNowSpeed( 0.0f );
	f_MoveBackDist = 0;
	uc_SlaCnt = 0;
	if(uc_trgX == GOAL_MAP_X && uc_trgY == GOAL_MAP_Y){
		f_MoveBackDist = MOVE_BACK_DIST;
	}
	
	log_flag_on();	//ログ関数スタート（大会時削除）
	
	/* 迷路探索 */
	while(1){
		MAP_refMousePos( en_Head );								// 座標更新
//		MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
		
		/* 超信地旋回探索 */
		if( SEARCH_TURN == en_search ){
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
//			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			if( TRUE == bl_type ){
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			MAP_makeMapData();												// 壁データから迷路データを作成			← ここでデータ作成をミスっている
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);						// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
			
			/* 次の区画へ移動 */
			if(( mx == uc_trgX ) && ( my == uc_trgY )){
				MAP_actGoal();										// ゴール時の動作
				break;
			}
			else{
				MAP_moveNextBlock(en_head, &bl_type);				// 次の区画へ移動			← ここで改めてリリースチェック＋壁再度作成＋等高線＋超信地旋回動作
			}
		}
		/* スラローム探索 */
		else if( SEARCH_SLA == en_search ){
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
//			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			if( TRUE == bl_type ){
				
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			if (st_Known.bl_known != TRUE) {
				MAP_makeMapData();		// 壁データから迷路データを作成
			}
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);			// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
			
			/* 次の区画へ移動 */
			if(( mx == uc_trgX ) && ( my == uc_trgY )){
				MAP_actGoal();									// ゴール時の動作
				break;
			}
			else{
//				MAP_moveNextBlock_Sla(en_head, &bl_type, FALSE );	// 次の区画へ移動			← ここで改めてリリースチェック＋壁再度作成＋等高線＋超信地旋回動作
				MAP_moveNextBlock_acc(en_head, &bl_type);
			}
		}
		/* 帰還探索 */
		else if (SEARCH_RETURN == en_search) {
			
			if( TRUE == bl_type ){
				
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			if (st_Known.bl_known != TRUE) {
				MAP_makeMapData();		// 壁データから迷路データを作成
			}			
			MAP_makeReturnContourMap(uc_staX,uc_staY);
			MAP_searchCmdList(uc_staX, uc_staY, en_Head, uc_goalX, uc_goalX, &en_endDir);
			uc_trgX = Return_X;
			uc_trgY = Return_Y;
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);	

			/* 次の区画へ移動 */
//			if ((us_Cmap[my][mx] == 0)||((g_SysMap[uc_trgY][uc_trgX]&0xf0) == 0xf0)) {
			if ((mx == 0)&&(my == 0)){
				MAP_actGoal();
				break;
			}
//			}
			else {
				MAP_moveNextBlock_acc(en_head, &bl_type);
			}
//			LED_count(uc_trgY);
		}
		if(front_wall_miss == TRUE){
			MOT_turn(MOT_R180);
			en_Head = (enMAP_HEAD_DIR)(((uint8_t)en_Head + 2) & (MAP_HEAD_DIR_MAX - 1));
			MOT_goHitBackWall();
			LL_mDelay(100);
			MOT_setTrgtSpeed(SEARCH_SPEED/4.0);		// 目標速度
			MOT_goBlock_FinSpeed( MOVE_BACK_DIST, 0.0 );//区画中央に戻り おそらく動作バグ
			LL_mDelay(100);
			MOT_setTrgtSpeed(SEARCH_SPEED);		// 目標速度

			MAP_make_refPos_MapData(en_Head);

			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);			// 等高線MAP法で進行方向を算出
			MAP_moveNextBlock_frontmiss(en_head, &bl_type);

			front_wall_miss = FALSE;
		}

		if(Min_in>6){
			MOT_goBlock_FinSpeed(0.5,0.0);
			MOT_turn(MOT_R180);	
			LL_mDelay(200);
			MOT_turn(MOT_R180);	
			LL_mDelay(200);
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			
			/* 迷路関連を初期化 */
			en_Head		= NORTH;
			mx			= 0;
			my			= 0;
			f_MoveBackDist = 0;
			break;
		}
		
		/* 途中で制御不能になった */
		if( SYS_isOutOfCtrl() == TRUE ){
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			
			/* 迷路関連を初期化 */
			en_Head		= NORTH;
			mx			= 0;
			my			= 0;
			f_MoveBackDist = 0;
			
			// DCMCは下位モジュールで既にクリアと緊急停止を行っている。
			break;
		}
	}
	SearchFlag = FALSE;
	LL_mDelay(1000);
//	SYS_setEnable( SYS_MODE );			// モード変更有効

}

void MAP_searchGoalKnown_AllSection(
	uint8_t 			uc_trgX, 		///< [in] 目標x座標
	uint8_t 			uc_trgY, 		///< [in] 目標y座標 
	enMAP_ACT_MODE 	en_type, 		///< [in] 探索方法
	enSEARCH_MODE	en_search 		///< [in] 探索方法
){
	enMAP_HEAD_DIR	en_head = NORTH;
	bool		bl_type = TRUE;			// 現在位置、FALSE: １区間前進状態、TURE:半区間前進状態
	enMAP_HEAD_DIR		en_endDir;
	
	uint8_t uc_goalX;
	uint8_t uc_goalY;
	uint8_t uc_staX;
	uint8_t uc_staY;
	
	SearchFlag = TRUE;

	if (en_search == SEARCH_RETURN){
		uc_goalX = uc_trgX;
		uc_goalY = uc_trgY;
		uc_staX = mx;
		uc_staY = my;
//		printf("mx%d,my%d\n", mx, my);
//		MAP_makeContourMap(uc_trgX, uc_trgY, en_type);
		MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
		MAP_searchCmdList(uc_staX, uc_staY, en_Head, uc_goalX, uc_goalX, &en_endDir);
		uc_trgX = Return_X;
		uc_trgY = Return_Y;
//		printf("goalx%d,goaly%d\n", Return_X, Return_Y);
//		MAP_showcountLog();
	}

//	SYS_setDisable( SYS_MODE );				// モード変更禁止

	MOT_setTrgtSpeed(SEARCH_SPEED);		// 目標速度
	MOT_setNowSpeed( 0.0f );
	f_MoveBackDist = 0;
	uc_SlaCnt = 0;
	if(uc_trgX == GOAL_MAP_X && uc_trgY == GOAL_MAP_Y){
		f_MoveBackDist = MOVE_BACK_DIST;
	}
	
	log_flag_on();	//ログ関数スタート（大会時削除）
	
	/* 迷路探索 */
	while(1){
		MAP_refMousePos( en_Head );								// 座標更新
//		MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
		
		/* 超信地旋回探索 */
		if( SEARCH_TURN == en_search ){
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			if( TRUE == bl_type ){
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			MAP_makeMapData();												// 壁データから迷路データを作成			← ここでデータ作成をミスっている
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);						// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
			
			/* 次の区画へ移動 */
			if(( mx == uc_trgX ) && ( my == uc_trgY )){
				MAP_actGoal();										// ゴール時の動作
				break;
			}
			else{
				MAP_moveNextBlock(en_head, &bl_type);				// 次の区画へ移動			← ここで改めてリリースチェック＋壁再度作成＋等高線＋超信地旋回動作
			}
		}
		/* スラローム探索 */
		else if( SEARCH_SLA == en_search ){
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			if( TRUE == bl_type ){
				
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			if (st_Known.bl_known != TRUE) {
				MAP_makeMapData();		// 壁データから迷路データを作成
			}
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);			// 等高線MAP法で進行方向を算出			← 誤ったMAPを作成
			
			/* 次の区画へ移動 */
			if(( mx == uc_trgX ) && ( my == uc_trgY )){
				MAP_actGoal();									// ゴール時の動作
				break;
			}
			else{
//				MAP_moveNextBlock_Sla(en_head, &bl_type, FALSE );	// 次の区画へ移動			← ここで改めてリリースチェック＋壁再度作成＋等高線＋超信地旋回動作
				MAP_moveNextBlock_acc(en_head, &bl_type);
			}
		}
		/* 帰還探索 */
		else if (SEARCH_RETURN == en_search) {
			
			if( TRUE == bl_type ){
				
				MOT_goBlock_FinSpeed( 0.5 + f_MoveBackDist, SEARCH_SPEED );		// 半区画前進(バックの移動量を含む)
			}
			if (st_Known.bl_known != TRUE) {
				MAP_makeMapData();		// 壁データから迷路データを作成
			}			
			MAP_makeReturnContourMap(uc_staX,uc_staY);
			MAP_searchCmdList(uc_staX, uc_staY, en_Head, uc_goalX, uc_goalX, &en_endDir);
			uc_trgX = Return_X;
			uc_trgY = Return_Y;
//			MAP_makeContourMap( uc_trgX, uc_trgY, en_type );		// 等高線マップを作る
			MAP_makeContourMap_queue(uc_trgX, uc_trgY, en_type);
			MAP_calcMouseDir(CONTOUR_SYSTEM, &en_head);	

			/* 次の区画へ移動 */
//			if ((us_Cmap[my][mx] == 0)||((g_SysMap[uc_trgY][uc_trgX]&0xf0) == 0xf0)) {
			if ((mx == 0)&&(my == 0)){
				MAP_actGoal();
				break;
			}
//			}
			else {
				MAP_moveNextBlock_acc(en_head, &bl_type);
			}
//			LED_count(uc_trgY);
		}
		if(Min_in>6){
			MOT_goBlock_FinSpeed(0.5,0.0);
			MOT_turn(MOT_R180);	
			LL_mDelay(200);
			MOT_turn(MOT_R180);	
			LL_mDelay(200);
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			
			/* 迷路関連を初期化 */
			en_Head		= NORTH;
			mx			= 0;
			my			= 0;
			f_MoveBackDist = 0;
			break;
		}

		
		/* 途中で制御不能になった */
		if( SYS_isOutOfCtrl() == TRUE ){
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			
			/* 迷路関連を初期化 */
			en_Head		= NORTH;
			mx			= 0;
			my			= 0;
			f_MoveBackDist = 0;
			
			// DCMCは下位モジュールで既にクリアと緊急停止を行っている。
			break;
		}
	}
	SearchFlag = FALSE;
	LL_mDelay(1000);
//	SYS_setEnable( SYS_MODE );			// モード変更有効

}

void MAP_clearMap_direction(void)
{
	uint16_t	x, y;
	uint8_t	uc_data;

	/* すべてのマップデータを未探索状態にする */
	for (y = 0; y < MAP_Y_SIZE; y++) {
		for (x = 0; x < MAP_X_SIZE; x++) {
			uc_data = 0x00;
			g_MapDirection[y][x] = uc_data;
		}
	}

}

void  MAP_makeContourMap_dijkstra_modoki(
	uint8_t uc_goalX, 			///< [in] ゴールX座標
	uint8_t uc_goalY, 			///< [in] ゴールY座標
	enMAP_ACT_MODE	en_type		///< [in] 計算方法（まだ未使用）
) {
	uint16_t		x, y, i;		// ループ変数
	uint16_t		uc_dase;		// 基準値
	uint16_t		uc_new;			// 新値
	uint16_t		uc_level;		// 等高線
	uint8_t		uc_wallData;	// 壁情報

	en_type = en_type;		// コンパイルワーニング回避（いずれ削除）

	MAP_clearMap_direction();

	/* 等高線マップを初期化する */
	for (i = 0; i < MAP_SMAP_MAX_VAL; i++) {
		us_Cmap[i / MAP_Y_SIZE][i & (MAP_X_SIZE - 1)] = MAP_SMAP_MAX_VAL*4 - 1;
	}
	/* 目標地点の等高線を0に設定 */
	us_Cmap[uc_goalY][uc_goalX] = 0;
	if (GOAL_SIZE == 4) {
		us_Cmap[uc_goalY + 1][uc_goalX] = 0;
		us_Cmap[uc_goalY][uc_goalX + 1] = 0;
		us_Cmap[uc_goalY + 1][uc_goalX + 1] = 0;
	}
	else if (GOAL_SIZE == 9){
		us_Cmap[uc_goalY+1][uc_goalX] = 0;
		us_Cmap[uc_goalY][uc_goalX+1] = 0;
		us_Cmap[uc_goalY+1][uc_goalX+1] = 0;
		us_Cmap[uc_goalY+2][uc_goalX] = 0;
		us_Cmap[uc_goalY+2][uc_goalX+1] = 0;
		us_Cmap[uc_goalY][uc_goalX+2] = 0;
		us_Cmap[uc_goalY+1][uc_goalX+2] = 0;
		us_Cmap[uc_goalY+2][uc_goalX+2] = 0;
	}

	if (mx > uc_max_x)uc_max_x = mx;
	if (my > uc_max_y)uc_max_y = my;
	uc_max_x = 32;
	uc_max_y = 32;

	g_MapDirection[uc_goalY][uc_goalX] = 0xff;

	/* 等高線マップを作成 */
	uc_dase = 0;
	do {
		uc_level = 0;
		uc_new = uc_dase + 1;
		for (y = 0; y < MAP_Y_SIZE; y++) {
			if (uc_max_y+1 < y) break;
			for (x = 0; x < MAP_X_SIZE; x++) {
				if (us_Cmap[y][x] == uc_dase) {
					uc_wallData = g_SysMap[y][x];
					if (uc_max_x+1 < x) break;
					/* 探索走行 */
					if (SEARCH == en_type) {
						if (((uc_wallData & 0x01) == 0x00) && (y != (MAP_Y_SIZE - 1))) {
							if (us_Cmap[y + 1][x] == MAP_SMAP_MAX_VAL - 1) {
								us_Cmap[y + 1][x] = uc_new;
								uc_level++;
							}
						}
						if (((uc_wallData & 0x02) == 0x00) && (x != (MAP_X_SIZE - 1))) {
							if (us_Cmap[y][x + 1] == MAP_SMAP_MAX_VAL - 1) {
								us_Cmap[y][x + 1] = uc_new;
								uc_level++;
							}
						}
						if (((uc_wallData & 0x04) == 0x00) && (y != 0)) {
							if (us_Cmap[y - 1][x] == MAP_SMAP_MAX_VAL - 1) {
								us_Cmap[y - 1][x] = uc_new;
								uc_level++;
							}
						}
						if (((uc_wallData & 0x08) == 0x00) && (x != 0)) {
							if (us_Cmap[y][x - 1] == MAP_SMAP_MAX_VAL - 1) {
								us_Cmap[y][x - 1] = uc_new;
								uc_level++;
							}
						}
					}
					/* 最短走行 */
					else {
						if (((uc_wallData & 0x11) == 0x10) && (y != (MAP_Y_SIZE - 1))) {
							if((g_MapDirection[y][x]&0x10) == 0x10){
								uc_new = uc_dase + 1;
							}else{
								uc_new = uc_dase + 2;
							}
							if (us_Cmap[y + 1][x] > uc_new) {
								us_Cmap[y + 1][x] = uc_new;
								g_MapDirection[y+1][x] |= 0x10;
								uc_level++;
							}
						}
						if (((uc_wallData & 0x22) == 0x20) && (x != (MAP_X_SIZE - 1))) {
							if((g_MapDirection[y][x]&0x40) == 0x40){
								uc_new = uc_dase + 1;
							}else{
								uc_new = uc_dase + 2;
							}
							if (us_Cmap[y][x + 1] > uc_new) {
								us_Cmap[y][x + 1] = uc_new;
								g_MapDirection[y][x+1] |= 0x40;
								uc_level++;
							}
						}
						if (((uc_wallData & 0x44) == 0x40) && (y != 0)) {
							if((g_MapDirection[y][x]&0x01) == 0x01){
								uc_new = uc_dase + 1;
							}else{
								uc_new = uc_dase + 2;
							}
							if (us_Cmap[y - 1][x] > uc_new) {
								us_Cmap[y - 1][x] = uc_new;
								g_MapDirection[y-1][x] |= 0x01;
								uc_level++;
							}
						}
						if (((uc_wallData & 0x88) == 0x80) && (x != 0)) {
							if((g_MapDirection[y][x]&0x04) == 0x04){
								uc_new = uc_dase + 1;
							}else{
								uc_new = uc_dase + 2;
							}
							if (us_Cmap[y][x - 1] > uc_new) {
								us_Cmap[y][x - 1] = uc_new;
								g_MapDirection[y][x-1] |= 0x04;
								uc_level++;
							}
						}
					}
				}
				if(uc_dase != 4095)uc_level++;
			}
		}
		uc_dase = uc_dase + 1;
	} while (uc_level != 0);

}

