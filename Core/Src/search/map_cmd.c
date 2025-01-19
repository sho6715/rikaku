/*
 * map_cmd.c
 *
 *  Created on: 2021/02/04
 *      Author: shohe
 */

#include "search/map_cmd.h"

#define LIST_NUM			( 4096 )				// コマンド走行のリスト数

uint8_t		dcom[LIST_NUM];					// 超地信旋回用
uint8_t		scom[LIST_NUM];					// スラローム用
uint8_t		tcom[LIST_NUM];					// 斜め走行用
uint16_t		us_totalCmd;				// トータルコマンド量

float		f_PosX;							// X座標
float		f_PosY;							// Y座標
int16_t		s_PosDir;						// 進行方向（[0]北 [1]北東 [2]東 [3]南東 [4]南 [5]南西 [6]西 [7]北西 ）

/* 走行コマンド */
typedef enum{

	STOP = 0,	// 停止コマンド
	
	/* 直進 */
	GO1,		// [  1]  1区間前進コマンド
	GO2,		// [  2]  2区間前進コマンド
	GO3,		// [  3]  3区間前進コマンド
	GO4,		// [  4]  4区間前進コマンド
	GO5,		// [  5]  5区間前進コマンド
	GO6,		// [  6]  6区間前進コマンド
	GO7,		// [  7]  7区間前進コマンド
	GO8,		// [  8]  8区間前進コマンド
	GO9,		// [  9]  9区間前進コマンド
	GO10,		// [ 10] 10区間前進コマンド
	GO11,		// [ 11] 11区間前進コマンド
	GO12,		// [ 12] 12区間前進コマンド
	GO13,		// [ 13] 13区間前進コマンド
	GO14,		// [ 14] 14区間前進コマンド
	GO15,		// [ 15] 15区間前進コマンド
	GO16,		// [ 16] 16区間前進コマンド
	GO17,		// [ 17] 17区間前進コマンド
	GO18,		// [ 18] 18区間前進コマンド
	GO19,		// [ 19] 19区間前進コマンド
	GO20,		// [ 20] 20区間前進コマンド
	GO21,		// [ 21] 21区間前進コマンド
	GO22,		// [ 22] 22区間前進コマンド
	GO23,		// [ 23] 23区間前進コマンド
	GO24,		// [ 24] 24区間前進コマンド
	GO25,		// [ 25] 25区間前進コマンド
	GO26,		// [ 26] 26区間前進コマンド
	GO27,		// [ 27] 27区間前進コマンド
	GO28,		// [ 28] 28区間前進コマンド
	GO29,		// [ 29] 29区間前進コマンド
	GO30,		// [ 30] 30区間前進コマンド
	GO31,		// [ 31] 31区間前進コマンド
	GO32,		// [ 32] 32区間前進コマンド
	GO33,		// [ 33] 33区間前進コマンド
	GO34,		// [ 34] 34区間前進コマンド
	GO35,		// [ 35] 35区間前進コマンド
	GO36,		// [ 36] 36区間前進コマンド
	GO37,		// [ 37] 37区間前進コマンド
	GO38,		// [ 38] 38区間前進コマンド
	GO39,		// [ 39] 39区間前進コマンド
	GO40,		// [ 40] 40区間前進コマンド
	GO41,		// [ 41] 41区間前進コマンド
	GO42,		// [ 42] 42区間前進コマンド
	GO43,		// [ 43] 43区間前進コマンド
	GO44,		// [ 44] 44区間前進コマンド
	GO45,		// [ 45] 45区間前進コマンド
	GO46,		// [ 46] 46区間前進コマンド
	GO47,		// [ 47] 47区間前進コマンド
	GO48,		// [ 48] 48区間前進コマンド
	GO49,		// [ 49] 49区間前進コマンド
	GO50,		// [ 50] 50区間前進コマンド
	GO51,		// [ 51] 51区間前進コマンド
	GO52,		// [ 52] 52区間前進コマンド
	GO53,		// [ 53] 53区間前進コマンド
	GO54,		// [ 54] 54区間前進コマンド
	GO55,		// [ 55] 55区間前進コマンド
	GO56,		// [ 56] 56区間前進コマンド
	GO57,		// [ 57] 57区間前進コマンド
	GO58,		// [ 58] 58区間前進コマンド
	GO59,		// [ 59] 59区間前進コマンド
	GO60,		// [ 60] 60区間前進コマンド
	GO61,		// [ 61] 61区間前進コマンド
	GO62,		// [ 62] 62区間前進コマンド
	GO63,		// [ 63] 63区間前進コマンド
	GO64,		// [ 64] 64区間前進コマンド
	GO65,		// [ 65] 65区間前進コマンド
	GO66,		// [ 66] 66区間前進コマンド
	GO67,		// [ 67] 67区間前進コマンド
	GO68,		// [ 68] 68区間前進コマンド
	GO69,		// [ 69] 69区間前進コマンド
	GO70,		// [ 70] 70区間前進コマンド
	GO71,		// [ 71] 71区間前進コマンド
	
	/* 超地信旋回 */
	R90,		// [ 72] 右90度右超信旋回コマンド
	L90,		// [ 73] 左90度左超信旋回コマンド
	R180,		// [ 74] 右180度超信旋回コマンド
	L180,		// [ 75] 左180度超信旋回コマンド

	/* スラローム */
	R90S,		// [ 76] 右90度旋回コマンド
	L90S,		// [ 77] 左90度旋回コマンド
	R90F,		// [ 78] 右高速90度旋回コマンド
	L90F,		// [ 79] 左高速90度旋回コマンド
	R180F,		// [ 80] 右高速180度旋回コマンド
	L180F,		// [ 81] 左高速180度旋回コマンド

	/* 斜め */
	NGO1,		// [ 82] 斜め 1区間前進コマンド
	NGO2,		// [ 83] 斜め 2区間前進コマンド
	NGO3,		// [ 84] 斜め 3区間前進コマンド
	NGO4,		// [ 85] 斜め 4区間前進コマンド
	NGO5,		// [ 86] 斜め 5区間前進コマンド
	NGO6,		// [ 87] 斜め 6区間前進コマンド
	NGO7,		// [ 88] 斜め 7区間前進コマンド
	NGO8,		// [ 89] 斜め 8区間前進コマンド
	NGO9,		// [ 90] 斜め 9区間前進コマンド
	NGO10,		// [ 91] 斜め10区間前進コマンド
	NGO11,		// [ 92] 斜め11区間前進コマンド
	NGO12,		// [ 93] 斜め12区間前進コマンド
	NGO13,		// [ 94] 斜め13区間前進コマンド
	NGO14,		// [ 95] 斜め14区間前進コマンド
	NGO15,		// [ 96] 斜め15区間前進コマンド
	NGO16,		// [ 97] 斜め16区間前進コマンド
	NGO17,		// [ 98] 斜め17区間前進コマンド
	NGO18,		// [ 99] 斜め18区間前進コマンド
	NGO19,		// [100] 斜め19区間前進コマンド
	NGO20,		// [101] 斜め20区間前進コマンド
	NGO21,		// [102] 斜め21区間前進コマンド
	NGO22,		// [103] 斜め22区間前進コマンド
	NGO23,		// [104] 斜め23区間前進コマンド
	NGO24,		// [105] 斜め24区間前進コマンド
	NGO25,		// [106] 斜め25区間前進コマンド
	NGO26,		// [107] 斜め26区間前進コマンド
	NGO27,		// [108] 斜め27区間前進コマンド
	NGO28,		// [109] 斜め28区間前進コマンド
	NGO29,		// [110] 斜め29区間前進コマンド
	NGO30,		// [111] 斜め30区間前進コマンド
	NGO31,		// [112] 斜め31区間前進コマンド
	NGO32,		// [113] 斜め32区間前進コマンド
	NGO33,		// [114] 斜め33区間前進コマンド
	NGO34,		// [115] 斜め34区間前進コマンド
	NGO35,		// [116] 斜め35区間前進コマンド
	NGO36,		// [117] 斜め36区間前進コマンド
	NGO37,		// [118] 斜め37区間前進コマンド
	NGO38,		// [119] 斜め38区間前進コマンド
	NGO39,		// [120] 斜め39区間前進コマンド
	NGO40,		// [121] 斜め40区間前進コマンド
	NGO41,		// [122] 斜め41区間前進コマンド
	NGO42,		// [123] 斜め42区間前進コマンド
	NGO43,		// [124] 斜め43区間前進コマンド
	NGO44,		// [125] 斜め44区間前進コマンド
	NGO45,		// [126] 斜め45区間前進コマンド
	NGO46,		// [127] 斜め46区間前進コマンド
	NGO47,		// [128] 斜め47区間前進コマンド
	NGO48,		// [129] 斜め48区間前進コマンド
	NGO49,		// [130] 斜め49区間前進コマンド
	NGO50,		// [131] 斜め50区間前進コマンド
	NGO51,		// [132] 斜め51区間前進コマンド
	NGO52,		// [133] 斜め52区間前進コマンド
	NGO53,		// [134] 斜め53区間前進コマンド
	NGO54,		// [135] 斜め54区間前進コマンド
	NGO55,		// [136] 斜め55区間前進コマンド
	NGO56,		// [137] 斜め56区間前進コマンド
	NGO57,		// [138] 斜め57区間前進コマンド
	NGO58,		// [139] 斜め58区間前進コマンド
	NGO59,		// [140] 斜め59区間前進コマンド
	NGO60,		// [141] 斜め60区間前進コマンド
	NGO61,		// [142] 斜め61区間前進コマンド
	NGO62,		// [143] 斜め62区間前進コマンド
	NGO63,		// [144] 斜め63区間前進コマンド
	NGO64,		// [145] 斜め64区間前進コマンド
	NGO65,		// [146] 斜め65区間前進コマンド
	NGO66,		// [147] 斜め66区間前進コマンド
	NGO67,		// [148] 斜め67区間前進コマンド
	NGO68,		// [149] 斜め68区間前進コマンド
	NGO69,		// [150] 斜め69区間前進コマンド
	NGO70,		// [151] 斜め70区間前進コマンド
	NGO71,		// [152] 斜め71区間前進コマンド

	RS45N,		// [153] 直進 -> 右45   -> 斜め
	LS45N,		// [154] 直進 -> 左45   -> 斜め
	RS135N,		// [155] 直進 -> 右135  -> 斜め
	LS135N,		// [156] 直進 -> 左135  -> 斜め
	RN45S,		// [157] 斜め -> 右45   -> 直進
	LN45S,		// [158] 斜め -> 左45   -> 直進
	RN135S,		// [159] 斜め -> 右135  -> 直進
	LN135S,		// [160] 斜め -> 左135  -> 直進
	RN90N,		// [161] 斜め -> 右90   -> 斜め
	LN90N,		// [162] 斜め -> 左90   -> 斜め

	CEND = 250,	// [250] コマンド終了

	MAP_CMD_MAX,
}enMAP_CMD;

typedef struct{
	enMAP_CMD	en_cmd;			// コマンド
	float		f_x0_x1;		// [0]/[1]のX座標加算値
	float		f_y0_y1;		// [0]/[1]のy座標加算値
	float		f_x2_x3;		// [2]/[3]のX座標加算値
	float		f_y2_y3;		// [2]/[3]のy座標加算値
	float		f_x4_x5;		// [4]/[5]のX座標加算値
	float		f_y4_y5;		// [4]/[5]のy座標加算値
	float		f_x6_x7;		// [6]/[7]のX座標加算値
	float		f_y6_y7;		// [6]/[7]のy座標加算値
	int16_t		s_dir;			// 進行方向（[0]北 [1]北東 [2]東 [3]南東 [4]南 [5]南西 [6]西 [7]北西 ）
}stMAP_SIM;

/* コマンドに応じた座標更新データ */
const stMAP_SIM st_PosData[] = {
	
	//	コマンド	[0]/[1]のX	[0]/[1]のY	[2]/[3]のX	[2]/[3]のY	[4]/[5]のX	[4]/[5]のY	[6]/[7]のX	[6]/[7]のY	方向
	{ R90,			0.5,		0.5,		0.5,		-0.5,		-0.5,		-0.5,		-0.5,		0.5,		+2 },		// [0]
	{ L90,			-0.5,		0.5,		0.5,		0.5,		0.5,		-0.5,		-0.5,		-0.5,		-2 },		// [1]
	{ R90S,			0.5,		0.5,		0.5,		-0.5,		-0.5,		-0.5,		-0.5,		0.5,		+2 },		// [2]
	{ L90S,			-0.5,		0.5,		0.5,		0.5,		0.5,		-0.5,		-0.5,		-0.5,		-2 },		// [3]
	{ RS45N,		0.25,		0.75,		0.75,		-0.25,		-0.25,		-0.75,		-0.75,		0.25,		+1 },		// [4]
	{ LS45N,		-0.25,		0.75,		0.75,		0.25,		0.25,		-0.75,		-0.75,		-0.25,		-1 },		// [5]
	{ RS135N,		0.75,		0.25,		0.25,		-0.75,		-0.75,		-0.25,		-0.25,		0.75,		+3 },		// [6]
	{ LS135N,		-0.75,		0.25,		0.25,		0.75,		0.75,		-0.25,		-0.25,		-0.75,		-3 },		// [7]
	{ RN45S,		0.75,		0.25,		0.25,		-0.75,		-0.75,		-0.25,		-0.25,		0.75,		+1 },		// [8]
	{ LN45S,		0.25,		0.75,		0.75,		-0.25,		-0.25,		-0.75,		-0.75,		0.25,		-1 },		// [9]
	{ RN135S,		0.75,		-0.25,		-0.25,		-0.75,		-0.75,		0.25,		0.25,		0.75,		+3 },		// [10]
	{ LN135S,		-0.25,		0.75,		0.75,		0.25,		0.25,		-0.75,		-0.75,		-0.25,		-3 },		// [11]
	{ RN90N,		0.5,		0,			0,			-0.5,		-0.5,		0,			0,			0.5,		+2 },		// [12]
	{ LN90N,		0,			0.5,		0.5,		0,			0,			-0.5,		-0.5,		0,			-2 },		// [13]
	{ GO1,			0,			0.5,		0.5,		0,			0,			-0.5,		-0.5,		0,			0  },		// [14]
	{ NGO1,			0.5,		0.5,		0.5,		-0.5,		-0.5,		-0.5,		-0.5,		0.5,		0  },		// [15]
	{ MAP_CMD_MAX,	0,			0,			0,			0,			0,			0,			0,			0,			0  },
};

float f_LogPosX[30];
float f_LogPosY[30];
uint16_t us_LogIndex = 0;
uint16_t us_LogWallCut[30];
uint16_t us_LogIndexWallCut = 0;

void MAP_refPos( uint8_t uc_cmd )
{
	uint8_t uc_index = 0;			// テーブルのインデックス番号
	
	/* ------------------------------------------ */
	/*  コマンドからテーブルのインデックスを取得  */
	/* ------------------------------------------ */
	/* 直進 */
	if( ( uc_cmd <=  GO71 ) && ( uc_cmd >=  GO1) ){
		
		uc_index = 14;		// st_PosDataテーブルの直進のインデックス番号
	}
	/* 斜め直進 */
	else if( ( uc_cmd <=  NGO71 ) && ( uc_cmd >=  NGO1) ){
		
		uc_index = 15;		// st_PosDataテーブルの斜め直進のインデックス番号
	}
	/* その他のコマンド */
	else{
		while(1){
			
			if( st_PosData[uc_index].en_cmd == uc_cmd )      break;			// コマンド発見
			if( st_PosData[uc_index].en_cmd == MAP_CMD_MAX ) return;		// コマンド未発見
			uc_index++;
		}
	}
	
		/* 位置更新 */
	switch( s_PosDir ){
		
		/* [0]北 [1]北東 */
		case 0:
		case 1:
		
			/* 直進 */
			if( uc_index == 14 ){
				
				f_PosX += st_PosData[uc_index].f_x0_x1 * uc_cmd;
				f_PosY += st_PosData[uc_index].f_y0_y1 * uc_cmd;
			}
			/* 斜め直進 */
			else if( uc_index == 15 ){
				
				f_PosX += st_PosData[uc_index].f_x0_x1 * ( uc_cmd - 81 );
				f_PosY += st_PosData[uc_index].f_y0_y1 * ( uc_cmd - 81 );
			}
			/* その他のカーブ */
			else{
				f_PosX += st_PosData[uc_index].f_x0_x1;
				f_PosY += st_PosData[uc_index].f_y0_y1;
			}
			break;
		
		/* [2]東 [3]南東 */
		case 2:
		case 3:

			/* 直進 */
			if( uc_index == 14 ){
				
				f_PosX += st_PosData[uc_index].f_x2_x3 * uc_cmd;
				f_PosY += st_PosData[uc_index].f_y2_y3 * uc_cmd;
			}
			/* 斜め直進 */
			else if( uc_index == 15 ){
				
				f_PosX += st_PosData[uc_index].f_x2_x3 * ( uc_cmd - 81 );
				f_PosY += st_PosData[uc_index].f_y2_y3 * ( uc_cmd - 81 );
			}
			/* その他のカーブ */
			else{
				f_PosX += st_PosData[uc_index].f_x2_x3;
				f_PosY += st_PosData[uc_index].f_y2_y3;
			}
			break;

		/* [4]南 [5]南西 */
		case 4:
		case 5:

			/* 直進 */
			if( uc_index == 14 ){
				
				f_PosX += st_PosData[uc_index].f_x4_x5 * uc_cmd;
				f_PosY += st_PosData[uc_index].f_y4_y5 * uc_cmd;
			}
			/* 斜め直進 */
			else if( uc_index == 15 ){
				
				f_PosX += st_PosData[uc_index].f_x4_x5 * ( uc_cmd - 81 );
				f_PosY += st_PosData[uc_index].f_y4_y5 * ( uc_cmd - 81 );
			}
			/* その他のカーブ */
			else{
				f_PosX += st_PosData[uc_index].f_x4_x5;
				f_PosY += st_PosData[uc_index].f_y4_y5;
			}
			break;

		/* [6]西 [7]北西 */
		case 6:
		case 7:

			/* 直進 */
			if( uc_index == 14 ){
				
				f_PosX += st_PosData[uc_index].f_x6_x7 * uc_cmd;
				f_PosY += st_PosData[uc_index].f_y6_y7 * uc_cmd;
			}
			/* 斜め直進 */
			else if( uc_index == 15 ){
				
				f_PosX += st_PosData[uc_index].f_x6_x7 * ( uc_cmd - 81 );
				f_PosY += st_PosData[uc_index].f_y6_y7 * ( uc_cmd - 81 );
			}
			/* その他のカーブ */
			else{
				f_PosX += st_PosData[uc_index].f_x6_x7;
				f_PosY += st_PosData[uc_index].f_y6_y7;
			}
			break;
	}
	
	/* 進行方向更新 */
	s_PosDir += st_PosData[uc_index].s_dir;
	if( s_PosDir < 0 ) s_PosDir += 8;				// [0]～[7]にしたい
	else if( s_PosDir > 7 ) s_PosDir -= 8;
	
	f_LogPosX[us_LogIndex] = f_PosX;
	f_LogPosY[us_LogIndex] = f_PosY;
	
	us_LogIndex++;
	us_LogIndex %= 30;
}

bool MAP_setWallCut( uint8_t uc_cmd )
{
	uint8_t uc_val = 0;			// 1区画前のコーナー側の壁があるか（0以外なら壁あり）
	uint8_t uc_valPrev = 0;		// 2区画前のコーナー側の壁があるか（0以外なら壁あり）
	bool bl_wallCut = FALSE;
	
	/* 位置更新 */
	switch( uc_cmd ){
		
		case R90S:
		case RS135N:
			
			/* 1区画前のコーナー側の壁があるか（0以外ならあり） */
			// s_PosDir：進行方向（[0]北 [1]北東 [2]東 [3]南東 [4]南 [5]南西 [6]西 [7]北西 ）
			switch( s_PosDir ){
				
				/* 柱基準で旋回するので、半区画手前が壁の有無を調べたい座標となる（注意：g_SysMapは2次元配列です） */
				case 0: 
					if( 0 < f_PosY-0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY-0.5)][(uint8_t)(f_PosX)] & 0x02;		// 北を向いているので東側の壁があるか
					if( 0 < f_PosY-1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY-1.5)][(uint8_t)(f_PosX)] & 0x02;		// 北を向いているので東側の壁があるか
					break;	
				case 2: 
					if( 0 < f_PosX-0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX-0.5)] & 0x04;		// 東を向いているので南側の壁があるか
					if( 0 < f_PosX-1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX-1.5)] & 0x04;		// 東を向いているので南側の壁があるか
					break;
				case 4: 
					if( MAP_Y_SIZE_REAL > f_PosY+0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY+0.5)][(uint8_t)(f_PosX)] & 0x08;		// 南を向いているので西側の壁があるか
					if( MAP_Y_SIZE_REAL > f_PosY+1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY+1.5)][(uint8_t)(f_PosX)] & 0x08;		// 南を向いているので西側の壁があるか
					break;
				case 6:
					if( MAP_X_SIZE_REAL > f_PosX+0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX+0.5)] & 0x01;		// 西を向いているので北側の壁があるか
					if( MAP_X_SIZE_REAL > f_PosX+1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX+1.5)] & 0x01;		// 西を向いているので北側の壁があるか
					break;
			}
			/* 壁があるため壁切れ補正を行う */
			if( ( uc_val != 0 ) || ( ( uc_val != 0 ) && ( uc_valPrev != 0 ) ) ){
				
				MOT_setWallEdgeType( MOT_WALL_EDGE_RIGHT );		// 壁切れ補正を実施する
				bl_wallCut = TRUE;
			}
			break;
			
		case L90S:
		case LS135N:
			/* 1区画前のコーナー側の壁があるか（0以外ならあり） */
			// s_PosDir：進行方向（[0]北 [1]北東 [2]東 [3]南東 [4]南 [5]南西 [6]西 [7]北西 ）
			switch( s_PosDir ){
				
				/* 柱基準で旋回するので、半区画手前が壁の有無を調べたい座標となる（注意：g_SysMapは2次元配列です） */
				case 0: 
					if( 0 < f_PosY-0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY-0.5)][(uint8_t)(f_PosX)] & 0x08;			// 北を向いているので西側の壁があるか
					if( 0 < f_PosY-1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY-1.5)][(uint8_t)(f_PosX)] & 0x08;			// 北を向いているので西側の壁があるか
					break;
				case 2: 
					if( 0 < f_PosX-0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX-0.5)] & 0x01;			// 東を向いているので北側の壁があるか
					if( 0 < f_PosX-1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX-1.5)] & 0x01;			// 東を向いているので北側の壁があるか
					break;
				case 4: 
					if( MAP_Y_SIZE_REAL > f_PosY+0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY+0.5)][(uint8_t)(f_PosX)] & 0x02;			// 南を向いているので東側の壁があるか
					if( MAP_Y_SIZE_REAL > f_PosY+1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY+1.5)][(uint8_t)(f_PosX)] & 0x02;			// 南を向いているので東側の壁があるか
					break;
				case 6: 
					if( MAP_X_SIZE_REAL > f_PosX+0.5 ) uc_val     = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX+0.5)] & 0x04;			// 西を向いているので南側の壁があるか
					if( MAP_X_SIZE_REAL > f_PosX+1.5 ) uc_valPrev = g_SysMap[(uint8_t)(f_PosY)][(uint8_t)(f_PosX+1.5)] & 0x04;			// 西を向いているので南側の壁があるか
					break;
			}
			/* 壁があるため壁切れ補正を行う */
			if( ( uc_val != 0 ) || ( ( uc_val != 0 ) && ( uc_valPrev != 0 ) ) ){
				
				MOT_setWallEdgeType( MOT_WALL_EDGE_LEFT );		// 壁切れ補正を実施する
				bl_wallCut = TRUE;
			}
			break;
			
		default:
			break;
	}
	
	return bl_wallCut;
}

void MAP_setCmdPos( uint8_t uc_x, uint8_t uc_y, enMAP_HEAD_DIR en_dir )
{
	f_PosX   = (float)uc_x;
	f_PosX   = (float)uc_y;
	s_PosDir = (int16_t)(en_dir * 2);	// 進行方向（[0]北 [1]北東 [2]東 [3]南東 [4]南 [5]南西 [6]西 [7]北西 ）、2倍すると丁度値が合致する
}

void MAP_showCmdLog( void )
{
	uint16_t i=0;
	
	/* 超信地旋回コマンド */
	while(1){
		
		printf("dcom[%4d] = %02d  \n\r",i,dcom[i]);
		if( dcom[i] == CEND ) break;
		i++;
	}
	i=0;
	
	/* スラロームコマンド */
	while(1){
		
		printf("scom[%4d] = %02d  \n\r",i,scom[i]);
		if( scom[i] == CEND ) break;
		i++;
	}
	i=0;

	/* 斜め走行コマンド */
	while(1){
		
		printf("tcom[%4d] = %02d  \n\r",i,tcom[i]);
		if( tcom[i] == CEND ) break;
		i++;
	}
}

void MAP_makeCmdList( 
	uint8_t uc_staX,					///< [in] 開始X座標
	uint8_t uc_staY,					///< [in] 開始Y座標
	enMAP_HEAD_DIR en_staDir,		///< [in] 開始時の方向
	uint8_t uc_endX,					///< [in] 終了X座標
	uint8_t uc_endY,					///< [in] 終了Y座標
	enMAP_HEAD_DIR* en_endDir		///< [out] 終了時の方向
){
	uint8_t			uc_goStep;									// 前進のステップ数
	uint16_t			us_high;								// 等高線の高さ
	uint16_t			us_pt;									// コマンドポインタ
	enMAP_HEAD_DIR	en_nowDir;									// 現在マウスの向いている絶対方向
	enMAP_HEAD_DIR	en_tempDir;									// 相対方向
//	uint16_t			i;											// roop
	
	/* 前進ステップ数を初期化する */
	uc_goStep = 0;
	us_pt = 0;

	/* 迷路情報からコマンド作成 */
	while(1){	
		us_high = us_Cmap[uc_staY][uc_staX]-1;
		if (en_staDir == NORTH){
			if     (((g_SysMap[uc_staY][uc_staX] & 0x11)==0x10)&&(us_Cmap[uc_staY+1][uc_staX]==us_high)) en_nowDir=NORTH;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x22)==0x20)&&(us_Cmap[uc_staY][uc_staX+1]==us_high)) en_nowDir=EAST;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x88)==0x80)&&(us_Cmap[uc_staY][uc_staX-1]==us_high)) en_nowDir=WEST;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x44)==0x40)&&(us_Cmap[uc_staY-1][uc_staX]==us_high)) en_nowDir=SOUTH;
			else   while(1);
		}else if (en_staDir == EAST){
			if     (((g_SysMap[uc_staY][uc_staX] & 0x22)==0x20)&&(us_Cmap[uc_staY][uc_staX+1]==us_high)) en_nowDir=EAST;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x11)==0x10)&&(us_Cmap[uc_staY+1][uc_staX]==us_high)) en_nowDir=NORTH;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x44)==0x40)&&(us_Cmap[uc_staY-1][uc_staX]==us_high)) en_nowDir=SOUTH;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x88)==0x80)&&(us_Cmap[uc_staY][uc_staX-1]==us_high)) en_nowDir=WEST;
			else   while(1);
		}else if (en_staDir == SOUTH){
			if     (((g_SysMap[uc_staY][uc_staX] & 0x44)==0x40)&&(us_Cmap[uc_staY-1][uc_staX]==us_high)) en_nowDir=SOUTH;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x22)==0x20)&&(us_Cmap[uc_staY][uc_staX+1]==us_high)) en_nowDir=EAST;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x88)==0x80)&&(us_Cmap[uc_staY][uc_staX-1]==us_high)) en_nowDir=WEST;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x11)==0x10)&&(us_Cmap[uc_staY+1][uc_staX]==us_high)) en_nowDir=NORTH;
			else   while(1);
		}else if (en_staDir == WEST){
			if     (((g_SysMap[uc_staY][uc_staX] & 0x88)==0x80)&&(us_Cmap[uc_staY][uc_staX-1]==us_high)) en_nowDir=WEST;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x11)==0x10)&&(us_Cmap[uc_staY+1][uc_staX]==us_high)) en_nowDir=NORTH;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x44)==0x40)&&(us_Cmap[uc_staY-1][uc_staX]==us_high)) en_nowDir=SOUTH;
			else if(((g_SysMap[uc_staY][uc_staX] & 0x22)==0x20)&&(us_Cmap[uc_staY][uc_staX+1]==us_high)) en_nowDir=EAST;
			else   while(1);
		}
		
		en_tempDir = (enMAP_HEAD_DIR)( (en_nowDir - en_staDir) & (enMAP_HEAD_DIR)3 );		// 方向更新
		en_staDir = en_nowDir;

		if (en_tempDir == NORTH){
			uc_goStep = uc_goStep + 2;
		}
		else if (en_tempDir == EAST){
			dcom[us_pt] = uc_goStep;
			dcom[++us_pt] = R90;
			uc_goStep = 2;
			us_pt++;
		}
		else if (en_tempDir == WEST){
			dcom[us_pt] = uc_goStep;
			dcom[++us_pt] = L90;
			uc_goStep = 2;
			us_pt++;
		}
		else{
			dcom[us_pt] = uc_goStep;
			dcom[++us_pt] = R180;
			uc_goStep = 2;
			us_pt++;
		}

		if      (en_nowDir == NORTH) uc_staY = uc_staY + 1;
		else if (en_nowDir == EAST) uc_staX = uc_staX + 1;
		else if (en_nowDir == SOUTH) uc_staY = uc_staY - 1;
		else if (en_nowDir == WEST) uc_staX = uc_staX - 1;
		
		en_staDir = en_nowDir;
		
		if ((uc_staX == uc_endX) &&(uc_staY == uc_endY)) break;
	}
	
	/* 超地信旋回用のコマンドリスト作成 */
	dcom[us_pt] = uc_goStep;
	dcom[++us_pt] = STOP;
	dcom[++us_pt] = CEND;
	us_totalCmd = us_pt+1;			// コマンド総数


	/* 最終的に向いている方向 */
	*en_endDir = en_staDir;
}

void MAP_makeSlaCmdList( void )
{
	uint16_t dcom_temp[4096];			// 半区画超信旋回コマンドリスト
	uint16_t i=0,j=0;					// roop
	
	/* 超地信旋回コマンドをコピー */
	for( i=0; i<us_totalCmd; i++ ){
		dcom_temp[i] = dcom[i];
	}

	i = 0;

	/* 配列が旋回コマンドかをチェック */
	while(1)
	{
		if( dcom_temp[i] == R90 ){		// 右90°
			dcom_temp[i-1] -= 1;		// 1つ手前を引く
			dcom_temp[i+1] -= 1;		// 1つ手前を引く
			dcom_temp[i] = R90S;		// 右スラローム90°
		}
		else if( dcom_temp[i] == L90 ){	// 左90°
			dcom_temp[i-1] -= 1;		// 1つ手前を引く
			dcom_temp[i+1] -= 1;		// 1つ手前を引く
			dcom_temp[i] = L90S;		// 左スラローム90°
		}
		else{
			if( dcom_temp[i] == CEND ){
				break;
			}
		}
		i++;
	}

	i = j = 0;

	/* スラロームコマンド変換 */
	while(1)
	{
		if( dcom_temp[i+1] == CEND ){
			scom[j] = STOP;
			scom[j+1] = CEND;
			break;
		}
		else
		{
			/* データがストップコマンドだったら */
			if( dcom_temp[i] == 0 ){
				i++;
			}
			
			scom[j] = dcom_temp[i];
			
			i++;
			j++;
		}
	}
}

void MAP_makeSkewCmdList( void )
{
	uint16_t	scom_temp[4096];			// 半区画超信旋回コマンドリスト
	uint16_t	i;							// roop
	uint16_t	c1, c2, c3, c4;				// 計算用
	uint16_t	x;
	uint16_t	ct_n=0, ct_st=0;
	uint16_t	flag = 3;					//	斜め走行用バッファ  0:複合コマンド　1:斜め  2:S135N → N135S  3:直進
	
	/* 超地信旋回コマンドをコピー */
	for( i=0; i<us_totalCmd; i++ )
	{
		scom_temp[i] = scom[i];
	}

	i=0;

	/* 配列が旋回コマンドかをチェック */
	while(1)
	{
		c1 = scom_temp[ct_st];
		c2 = scom_temp[ct_st+1];
		c3 = scom_temp[ct_st+2];
		c4 = scom_temp[ct_st+3];

		//	直進 → 右45度 → 斜め
		if( (c1<=GO32) && (c2==R90S) && (c3==L90S) )
		{
			if((ct_st == 0) && (c1-1 == 0)){
				tcom[ ct_n ] = scom_temp[ct_st];
				ct_st ++;
				ct_n ++;
			}
			else{
				if( c1-1 != 0 ) tcom[ ct_n++ ] = c1 - 1;		//	前の複合コマンドによって直線区間が消えない場合
				tcom[ ct_n++ ] = RS45N;
				ct_st ++;

				x = (uint16_t)(NGO1 - 1);		//	斜めモード
				flag = 0;
			}
		}
		//	直進 → 左45度 → 斜め
		else if( (c1<=GO32) && (c2==L90S) && (c3==R90S) )
		{
			if((ct_st == 0) && (c1-1 == 0)){
				tcom[ ct_n ] = scom_temp[ct_st];
				ct_st ++;
				ct_n ++;
			}
			else{
				if( c1-1 != 0 ) tcom[ ct_n++ ] = c1 - 1;		//	前の複合コマンドによって直線区間が消えない場合
				tcom[ ct_n++ ] = LS45N;
				ct_st ++;

				x = (uint16_t)(NGO1 - 1);		//	斜めモード
				flag = 0;
			}
		}

		//	直進 → 右90度 → 直進
		else if( (c1<=GO32) && (c2==R90S) && (c3<=GO32) )
		{
			tcom[ ct_n++ ] = c1;
			tcom[ ct_n++ ] = R90S;
			ct_st += 2;
			flag = 3;		//	直進
		}
		//	直進 → 左90度 → 直進
		else if( (c1<=GO32) && (c2==L90S) && (c3<=GO32) )
		{
			tcom[ ct_n++ ] = c1;
			tcom[ ct_n++ ] = L90S;
			ct_st += 2;
			flag = 3;		//	直進
		}
		//	直進 → 右135度 → 斜め
		else if( (c1<=GO32) && (c2==R90S) && (c3==R90S) && (c4==L90S) )
		{
			tcom[ ct_n++ ] = c1;
			tcom[ ct_n++ ] = RS135N;
			ct_st += 2;

			x = (uint16_t)(NGO1 - 1);		//	斜めモード
			flag = 2;
		}
		//	直進 → 左135度 → 斜め
		else if( (c1<=GO32) && (c2==L90S) && (c3==L90S) && (c4==R90S) )
		{
			tcom[ ct_n++ ] = c1;
			tcom[ ct_n++ ] = LS135N;
			ct_st += 2;

			x = (uint16_t)(NGO1 - 1);		//	斜めモード
			flag = 2;
		}

		//	直進 → 右180度 → 直進
		else if( (c1<=GO32) && (c2==R90S) && (c3==R90S) && (c4<=GO32) )
		{
			tcom[ ct_n++ ] = c1;
			tcom[ ct_n++ ] = R90S;
			tcom[ ct_n++ ] = R90S;
			ct_st += 3;
			flag = 3;		//	直進
		}
		//	直進 → 左180度 → 直進
		else if( (c1<=GO32) && (c2==L90S) && (c2==L90S) && (c4<=GO32) )
		{
			tcom[ ct_n++ ] = c1;
			tcom[ ct_n++ ] = L90S;
			tcom[ ct_n++ ] = L90S;
			ct_st += 3;
			flag = 3;		//	直進
		}

		//	斜め → 右45度 → 直進
		else if( (c1==R90S) && (c2<=GO32)  && (flag != 3 ) )
		{
			if( flag==1 ) tcom[ ct_n++ ] = x;
			tcom[ ct_n++ ] = RN45S;
			scom_temp[ct_st+1] = c2 - 1;		//	直線区間を1つ減らす
			ct_st ++;
			flag = 3;		//	直進
		}
		//	斜め → 左45度 → 直進
		else if( (c1==L90S) && (c2<=GO32)  && (flag != 3 ) )
		{
			if( flag==1 ) tcom[ ct_n++ ] = x;
			tcom[ ct_n++ ] = LN45S;
			scom_temp[ct_st+1] = c2 - 1;		//	直線区間を1つ減らす
			ct_st ++;
			flag = 3;		//	直進
		}
		//	斜め → 右90度 → 斜め
		else if( (c1==L90S) && (c2==R90S) && (c3==R90S) && (c4==L90S)  && (flag != 3 ) )
		{
			if( flag==0 ) tcom[ ct_n++ ] = NGO1;		//	45NからRN90N
			else if( flag==1 ) tcom[ ct_n++ ] = x+1;
			else if( flag==2 ) tcom[ ct_n++ ] = NGO1;
			tcom[ ct_n++ ] = RN90N;
			ct_st +=2;

			x = (uint16_t)(NGO1 - 1);		//	斜めモード
			flag = 1;
		}
		//	斜め → 左90度 → 斜め
		else if( (c1==R90S) && (c2==L90S) && (c3==L90S) && (c4==R90S)  && (flag != 3 ) )
		{
			if( flag==0 ) tcom[ ct_n++ ] = NGO1;		//	45NからLN90N
			else if( flag==1 ) tcom[ ct_n++ ] = x+1;
			else if( flag==2 ) tcom[ ct_n++ ] = NGO1;
			tcom[ ct_n++ ] = LN90N;
			ct_st +=2;

			x = (uint16_t)(NGO1 - 1);		//	斜めモード
			flag = 1;
		}
		//	斜め → 右135度 → 直進
		else if( (c1==L90S) && (c2==R90S) && (c3==R90S) && (c4<=GO32)  && (flag != 3 ) )
		{
			if( flag==0 ) tcom[ ct_n++ ] = NGO1;		//	45NからLN90N
			else if( flag==1 ) tcom[ ct_n++ ] = x+1;
			else if( flag==2 ) tcom[ ct_n++ ] = NGO1;
			tcom[ ct_n++ ] = RN135S;
			ct_st += 3;
			flag = 3;		//	直進
		}
		//	斜め → 左135度 → 直進
		else if( (c1==R90S) && (c2==L90S) && (c3==L90S) && (c4<=GO32)  && (flag != 3 ) )
		{
			if( flag==0 ) tcom[ ct_n++ ] = NGO1;		//	45NからLN90N
			else if( flag==1 ) tcom[ ct_n++ ] = x+1;
			else if( flag==2 ) tcom[ ct_n++ ] = NGO1;
			tcom[ ct_n++ ] = LN135S;
			ct_st += 3;
			flag = 3;		///	直進
		}
		//	斜め → 斜め
		else if( (c1==R90S) && (c2==L90S) && ( (c3==R90S) || (c3==L90S) || ( c3<=GO32 ) ) && (flag != 3 ) )
		{
			x++;
			ct_st ++;

			flag = 1;		//	斜め走行バッファあり
		}
		else if( (c1==L90S) && (c2==R90S) && ( (c3==L90S) || (c3==R90S) || ( c3<=GO32 ) ) && (flag != 3 ) )
		{
			//	コマンド出力
			x++;
			ct_st ++;

			flag = 1;		//	斜め走行バッファあり
		}
		else
		{
			tcom[ ct_n ] = scom_temp[ct_st];
			if( tcom[ ct_n ] == CEND ) break;
			ct_st ++;
			ct_n ++;
		}
	}
}

void MAP_drive( enMAP_DRIVE_TYPE en_driveType )
{
	uint16_t			us_rp = 0;				// 現在の読み込み位置
	enMOT_TURN_CMD 		en_type;
	bool			bl_isWallCut = FALSE;
	
	/* 超信旋回モード*/
	if( en_driveType == MAP_DRIVE_TURN )
	{
		while(1)
		{
			if ( dcom[us_rp] == CEND  ) break;								//	コマンド終了
			
			else if ( dcom[us_rp] == STOP  ){
			 	CTRL_stop();			// 制御停止
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
			}
			else if ( ( dcom[us_rp] <=  GO71 ) && ( dcom[us_rp] >=  GO1) )
			{
				MOT_goBlock_FinSpeed( (float)dcom[us_rp]*0.5f, 0 );		// 直線走行コマンド、半区間前進後に停止
			}
			else{
				
				if( dcom[us_rp] == R90 ) en_type = MOT_R90;
				else 					 en_type = MOT_L90;
				
				LL_mDelay(500);
				MOT_turn( en_type );		//	旋回
				LL_mDelay(500);
			}
			us_rp++;
			
			/* 途中で制御不能になった */
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot(DCM_R);
				DCM_brakeMot(DCM_L);
				break;
			}
			
		}
	 	CTRL_stop();			// 制御停止
		DCM_brakeMot( DCM_R );		// ブレーキ
		DCM_brakeMot( DCM_L );		// ブレーキ
	}
	/* スラロームモード */
	else if( en_driveType == MAP_DRIVE_SLA )
	{
		while(1)
		{
			MAP_refPos( scom[us_rp] );									// 実行されるコマンドが終了した位置に更新

			if ( scom[us_rp] == CEND  ) break;							//	コマンド終了
			
			else if ( scom[us_rp] == STOP  )
			{
			 	CTRL_stop();			// 制御停止
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
			}
			else if ( ( scom[us_rp] <=  GO71 ) && ( scom[us_rp] >=  GO1) )
			{
				if( scom[us_rp+1] == STOP  ){
					MOT_goBlock_FinSpeed( (float)scom[us_rp]*0.5f, 0 );						// 直線走行コマンド、半区間前進（最終速度なし）
				}
				else{
					
					/* 壁の切れ目補正 */
					if( ( scom[us_rp+1] == R90S )   || ( scom[us_rp+1] == L90S ) ){
						bl_isWallCut = MAP_setWallCut( scom[us_rp+1] );		// コーナー前に壁があったら壁の切れ目補正を行う設定をする
						
						if( bl_isWallCut == TRUE ){
							
							bl_isWallCut = FALSE;
							us_LogWallCut[us_LogIndexWallCut] = us_rp;
							us_LogIndexWallCut++;
							us_LogIndexWallCut %= 30;
						}
					}
					MOT_goBlock_FinSpeed( (float)scom[us_rp]*0.5f, MOT_getSlaStaSpeed(SLA_90) );		// 直線走行コマンド、半区間前進（最終速度あり）
				}
			}
			else if( scom[us_rp] == R90S )
			{
				MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );	// 右スラローム
			}
			else if( scom[us_rp] == L90S )
			{
				MOT_goSla( MOT_L90S, PARAM_getSra( SLA_90 ) );	// 左スラローム
			}
			us_rp++;
			
			/* 途中で制御不能になった */
			if( SYS_isOutOfCtrl() == TRUE){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}
			
		}
	}
	/* 斜めモード */
	else if( en_driveType == MAP_DRIVE_SKEW )
	{
		while(1)
		{
			MAP_refPos( tcom[us_rp] );									// 実行されるコマンドが終了した位置に更新
			
			if ( tcom[us_rp] == CEND  ) break;							//	コマンド終了

			else if ( tcom[us_rp] == STOP  )
			{
			 	CTRL_stop();			// 制御停止
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
			}
			else if ( ( tcom[us_rp] <=  GO71 ) && ( tcom[us_rp] >=  GO1) )
			{
				if( tcom[us_rp+1] == STOP  ){
					MOT_goBlock_FinSpeed( (float)tcom[us_rp]*0.5f, 0 );						// 直線走行コマンド、半区間前進（最終速度なし）
				}
				else{
					
					/* 壁の切れ目補正 */
					if( ( tcom[us_rp+1] == R90S )   || ( tcom[us_rp+1] == L90S )   || 
					 	( tcom[us_rp+1] == RS135N ) || ( tcom[us_rp+1] == LS135N ) 
					 ){
						bl_isWallCut = MAP_setWallCut( tcom[us_rp+1] );		// コーナー前に壁があったら壁の切れ目補正を行う設定をする
						
						if( bl_isWallCut == TRUE ){
							
							bl_isWallCut = FALSE;
							us_LogWallCut[us_LogIndexWallCut] = us_rp;
							us_LogIndexWallCut++;
							us_LogIndexWallCut %= 30;
						}
					}
					if((tcom[us_rp+1]==R90S)||(tcom[us_rp+1]==L90S)){
						MOT_goBlock_FinSpeed( (float)tcom[us_rp]*0.5f, MOT_getSlaStaSpeed(SLA_90) );		// 直線走行コマンド、半区間前進（最終速度あり）
					}else if((tcom[us_rp+1]==RS45N)||(tcom[us_rp+1]==LS45N)){
						MOT_goBlock_FinSpeed( (float)tcom[us_rp]*0.5f, MOT_getSlaStaSpeed(SLA_45) );		// 直線走行コマンド、半区間前進（最終速度あり）
					}else if((tcom[us_rp+1]==RS135N)||(tcom[us_rp+1]==LS135N)){
						MOT_goBlock_FinSpeed( (float)tcom[us_rp]*0.5f, MOT_getSlaStaSpeed(SLA_135) );		// 直線走行コマンド、半区間前進（最終速度あり）
					}
				}
			}
			else if ( ( tcom[us_rp] <=  NGO71 ) && ( tcom[us_rp] >=  NGO1) )
			{
				if((tcom[us_rp+1]==RN45S)||(tcom[us_rp+1]==LN45S)){
					MOT_goSkewBlock_FinSpeed( (float)(tcom[us_rp]-81)*0.5f, MOT_getSlaStaSpeed(SLA_45) );		// 直線走行コマンド、半区間前進（最終速度あり）
				}else if((tcom[us_rp+1]==RN135S)||(tcom[us_rp+1]==LN135S)){
					MOT_goSkewBlock_FinSpeed( (float)(tcom[us_rp]-81)*0.5f, MOT_getSlaStaSpeed(SLA_135) );		// 直線走行コマンド、半区間前進（最終速度あり）
				}else if((tcom[us_rp+1]==RN90N)||(tcom[us_rp+1]==LN90N)){
					MOT_goSkewBlock_FinSpeed( (float)(tcom[us_rp]-81)*0.5f, MOT_getSlaStaSpeed(SLA_N90) );		// 直線走行コマンド、半区間前進（最終速度あり）
				}
			}
			else
			{
				switch( tcom[us_rp] )
				{

					/* 直進 → 直進 */
					case R90S:		MOT_goSla( MOT_R90S, PARAM_getSra( SLA_90 ) );			break;
					case L90S:		MOT_goSla( MOT_L90S, PARAM_getSra( SLA_90 ) );			break;
					
					/* 直進 → 斜め */
					case RS45N:		MOT_goSla( MOT_R45S_S2N, PARAM_getSra( SLA_45 ) ); 		break;
					case LS45N:		MOT_goSla( MOT_L45S_S2N, PARAM_getSra( SLA_45 ) ); 		break;
					case RS135N:	MOT_goSla( MOT_R135S_S2N, PARAM_getSra( SLA_135 ) ); 	break;
					case LS135N:	MOT_goSla( MOT_L135S_S2N, PARAM_getSra( SLA_135 ) ); 	break;

					/* 斜め → 直進 */
					case RN45S:		MOT_goSla( MOT_R45S_N2S, PARAM_getSra( SLA_45 ) ); 		break;
					case LN45S:		MOT_goSla( MOT_L45S_N2S, PARAM_getSra( SLA_45 ) ); 		break;
					case RN135S:	MOT_goSla( MOT_R135S_N2S, PARAM_getSra( SLA_135 ) ); 	break;
					case LN135S:	MOT_goSla( MOT_L135S_N2S, PARAM_getSra( SLA_135 ) ); 	break;

					/* 斜め → 斜め */
					case RN90N:		MOT_goSla( MOT_R90S_N, PARAM_getSra( SLA_N90 ) ); 		break;
					case LN90N:		MOT_goSla( MOT_L90S_N, PARAM_getSra( SLA_N90 ) );		break;
				}
			}
			us_rp++;
			
			/* 途中で制御不能になった */
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}
		}
	}
}

void MAP_searchCmdList(
	uint8_t uc_staX,					///< [in] 開始X座標
	uint8_t uc_staY,					///< [in] 開始Y座標
	enMAP_HEAD_DIR en_staDir,		///< [in] 開始時の方向
	uint8_t uc_endX,					///< [in] 終了X座標
	uint8_t uc_endY,					///< [in] 終了Y座標
	enMAP_HEAD_DIR* en_endDir		///< [out] 終了時の方向
) {
	uint8_t			uc_goStep;									// 前進のステップ数
	uint16_t			us_high;								// 等高線の高さ
	uint16_t			us_pt;									// コマンドポインタ
	enMAP_HEAD_DIR	en_nowDir;									// 現在マウスの向いている絶対方向
	enMAP_HEAD_DIR	en_tempDir;									// 相対方向
//	uint16_t			i;											// roop

	/* 前進ステップ数を初期化する */
	uc_goStep = 0;
	us_pt = 0;
//	printf("mx%d,my%d\n", uc_staX, uc_staY);
	/* 迷路情報からコマンド作成 */
	while (1) {
		us_high = us_Cmap[uc_staY][uc_staX] - 1;
		if ((g_SysMap[uc_staY][uc_staX]&0xf0) != 0xf0){
			Return_X = uc_staX;
			Return_Y = uc_staY;
			break;
		}

		if (en_staDir == NORTH) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] == us_high)) en_nowDir = NORTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] == us_high)) en_nowDir = EAST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] == us_high)) en_nowDir = WEST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] == us_high)) en_nowDir = SOUTH;
			else   while (1);
		}
		else if (en_staDir == EAST) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] == us_high)) en_nowDir = EAST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] == us_high)) en_nowDir = NORTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] == us_high)) en_nowDir = SOUTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] == us_high)) en_nowDir = WEST;
			else   while (1);
		}
		else if (en_staDir == SOUTH) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] == us_high)) en_nowDir = SOUTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] == us_high)) en_nowDir = EAST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] == us_high)) en_nowDir = WEST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] == us_high)) en_nowDir = NORTH;
			else   while (1);
		}
		else if (en_staDir == WEST) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] == us_high)) en_nowDir = WEST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] == us_high)) en_nowDir = NORTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] == us_high)) en_nowDir = SOUTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] == us_high)) en_nowDir = EAST;
			else   while (1);
		}

		en_tempDir = (enMAP_HEAD_DIR)((en_nowDir - en_staDir) & (enMAP_HEAD_DIR)3);		// 方向更新
		en_staDir = en_nowDir;

		if (en_nowDir == NORTH) uc_staY = uc_staY + 1;
		else if (en_nowDir == EAST) uc_staX = uc_staX + 1;
		else if (en_nowDir == SOUTH) uc_staY = uc_staY - 1;
		else if (en_nowDir == WEST) uc_staX = uc_staX - 1;

		en_staDir = en_nowDir;

//		if ((uc_staX == uc_endX) && (uc_staY == uc_endY)) break;
		if (us_Cmap[uc_staY][uc_staX] == 0) {
			Return_X = 0;
			Return_Y = 0;
			break;
		}
	}


	/* 最終的に向いている方向 */
	*en_endDir = en_staDir;
}


void MAP_makeCmdList_dijkstra_modoki(
	uint8_t uc_staX,					///< [in] 開始X座標
	uint8_t uc_staY,					///< [in] 開始Y座標
	enMAP_HEAD_DIR en_staDir,		///< [in] 開始時の方向
	uint8_t uc_endX,					///< [in] 終了X座標
	uint8_t uc_endY,					///< [in] 終了Y座標
	enMAP_HEAD_DIR* en_endDir		///< [out] 終了時の方向
) {
	uint8_t			uc_goStep;									// 前進のステップ数
	uint16_t			us_high;									// 等高線の高さ
	uint16_t			us_pt;										// コマンドポインタ
	enMAP_HEAD_DIR	en_nowDir;									// 現在マウスの向いている絶対方向
	enMAP_HEAD_DIR	en_tempDir;									// 相対方向
//	USHORT			i;											// roop

	/* 前進ステップ数を初期化する */
	uc_goStep = 0;
	us_pt = 0;

	/* 迷路情報からコマンド作成 */
	while (1) {
		us_high = us_Cmap[uc_staY][uc_staX];

		if ((uc_staX == uc_endX) && (uc_staY == uc_endY)) {
			break;
		}
		
		if (en_staDir == NORTH) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] < us_high)) en_nowDir = NORTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] < us_high)) en_nowDir = EAST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] < us_high)) en_nowDir = WEST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] < us_high)) en_nowDir = SOUTH;
			else   while (1);
		}
		else if (en_staDir == EAST) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] < us_high)) en_nowDir = EAST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] < us_high)) en_nowDir = NORTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] < us_high)) en_nowDir = SOUTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] < us_high)) en_nowDir = WEST;
			else   while (1);
		}
		else if (en_staDir == SOUTH) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] < us_high)) en_nowDir = SOUTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] < us_high)) en_nowDir = EAST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] < us_high)) en_nowDir = WEST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] < us_high)) en_nowDir = NORTH;
			else   while (1);
		}
		else if (en_staDir == WEST) {
			if (((g_SysMap[uc_staY][uc_staX] & 0x88) == 0x80) && (us_Cmap[uc_staY][uc_staX - 1] < us_high)) en_nowDir = WEST;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x11) == 0x10) && (us_Cmap[uc_staY + 1][uc_staX] < us_high)) en_nowDir = NORTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x44) == 0x40) && (us_Cmap[uc_staY - 1][uc_staX] < us_high)) en_nowDir = SOUTH;
			else if (((g_SysMap[uc_staY][uc_staX] & 0x22) == 0x20) && (us_Cmap[uc_staY][uc_staX + 1] < us_high)) en_nowDir = EAST;
			else   while (1);
		}

		en_tempDir = (enMAP_HEAD_DIR)((en_nowDir - en_staDir) & (enMAP_HEAD_DIR)3);		// 方向更新
		en_staDir = en_nowDir;

		if (en_tempDir == NORTH) {
			uc_goStep = uc_goStep + 2;
		}
		else if (en_tempDir == EAST) {
			dcom[us_pt] = uc_goStep;
			dcom[++us_pt] = R90;
			uc_goStep = 2;
			us_pt++;
		}
		else if (en_tempDir == WEST) {
			dcom[us_pt] = uc_goStep;
			dcom[++us_pt] = L90;
			uc_goStep = 2;
			us_pt++;
		}
		else {
			dcom[us_pt] = uc_goStep;
			dcom[++us_pt] = R180;
			uc_goStep = 2;
			us_pt++;
		}

		if (en_nowDir == NORTH) uc_staY = uc_staY + 1;
		else if (en_nowDir == EAST) uc_staX = uc_staX + 1;
		else if (en_nowDir == SOUTH) uc_staY = uc_staY - 1;
		else if (en_nowDir == WEST) uc_staX = uc_staX - 1;

		en_staDir = en_nowDir;
/*
		if ((uc_staX == uc_endX) && (uc_staY == uc_endY)) {
			break;
		}
*/
//		if (us_Cmap[uc_staY][uc_staX] == 0) break;
	}

	/* 超地信旋回用のコマンドリスト作成 */
	dcom[us_pt] = uc_goStep;
	dcom[++us_pt] = STOP;
	dcom[++us_pt] = CEND;
	us_totalCmd = us_pt + 1;			// コマンド総数


	/* 最終的に向いている方向 */
	*en_endDir = en_staDir;

}
