/*
 * run.c
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */


#include "hal/run.h"

#define MOT_MOVE_ST_THRESHOLD			( 0.013 )							// 直進移動距離の閾値[mm]
#define MOT_MOVE_ST_MIN					( 0.010 )							// 直進移動距離の最低移動量[mm]

//20170815 超信地旋回実装時に追加
#define A1_MIN					( 25.0f/180.0f*PI )						// 第1最低移動角度
#define A2_MIN					( 30.0f/180.0f*PI )						// 第2最低移動角度
#define A3_MIN					( 20.0f/180.0f*PI )						// 第3最低移動角度

#define ANGLE_OFFSET1_R				( 0 )	//-12					// 角度のオフセット値（バッファリングによる誤差を埋めるための値）
#define ANGLE_OFFSET1				( 0 )	//-12					// 角度のオフセット値（バッファリングによる誤差を埋めるための値）
#define ANGLE_OFFSET2_R				( 0 )	//3
#define ANGLE_OFFSET2				( 0 )						// 角度のオフセット値（バッファリングによる誤差を埋めるための値）
#define ANGLE_OFFSET3				( 0 )					// 角度のオフセット値（バッファリングによる誤差を埋めるための値）

#define IS_R_SLA(a)			( ( (a) % 2 == 0 ) ? (TRUE) : (FALSE))

typedef struct{

	float			f_mot_time;			// 時間					[msec]
	/* 加速度制御 */
	float			f_mot_jerk;
	float			f_mot_trgtAcc1;
	float			f_mot_trgtAcc3;
	float			f_mot_nowAcc;
	float			f_mot_trgtAcc;
	float			f_mot_lastAcc;

	/* 速度制御 */
//	float			f_mot_acc1;			// [m/s2]
//	float			f_mot_acc3;			// [m/s2]
	float			f_mot_now;			// [m/s]
	float			f_mot_trgt;			// [m/s]
	float			f_mot_last;			// [m/s]

	float			f_mot_accjerk_v;
	float			f_mot_decjerk_v;

	/* 距離制御 */
	float			f_mot_dist;			// 移動距離				[m]
	float			f_mot_l1;			// 第1移動距離			[m]
	float			f_mot_l1_accjerk;	//　第1移動プラスジャーク
	float			f_mot_l1_accconst;	//　第1移動一定ジャーク
	float			f_mot_l1_decjerk;	//　第1移動マイナスジャーク
	float			f_mot_l1_2;			// 第1+2移動距離		[m]
	float			f_mot_l3_decjerk;	//　第3移動マイナスジャーク
	float			f_mot_l3_decconst;	//　第3移動一定ジャーク
	float			f_mot_l3_accjerk;	//　第3移動プラスジャーク

	float			f_mot_jerkAngle;
	float			f_mot_trgtAccAngle1;
	float			f_mot_trgtAccAngle3;
	float			f_mot_nowAccAngle;
	float			f_mot_trgtAccAngle;
	float			f_mot_lastAccAngle;

	float			f_mot_accAnglejerk_v;

	/* 角速度制御 */
//	float			f_mot_accAngleS1;	// 角加速度1			[rad/s2]
//	float			f_mot_accAngleS3;	// 角加速度3			[rad/s2]
	float			f_mot_nowAngleS;	// 現在角速度			[rad/s]
	float			f_mot_trgtAngleS;	// [rad/s]
	float			f_mot_lastAngleS;	// [rad/s]

	/* 角度制御 */
	float			f_mot_angle;		// 移動角度				[rad]
	float			f_mot_angle1;		// 第1移動角度			[rad]
	float			f_mot_angle1_2;		// 第1+2移動角度		[rad]
	float			f_mot_l1_accanglejerk;	//　第1移動プラスジャーク
	float			f_mot_l1_accangleconst;	//　第1移動一定ジャーク
	float			f_mot_l1_decanglejerk;	//　第1移動マイナスジャーク
	float			f_mot_l3_decanglejerk;	//　第3移動マイナスジャーク
	float			f_mot_l3_decangleconst;	//　第3移動一定ジャーク
	float			f_mot_l3_accanglejerk;	//　第3移動プラスジャーク


}stMOT_DATA;

stMOT_DATA 		st_Info;				// シーケンスデータ
/* 動作 */
float 			f_MotNowSpeed 		= 0.0f;		// 現在速度
float 			f_MotTrgtSpeed 		= 0.0f;		// 目標速度
float			f_MotSlaStaSpeed_90S	= 0.0f;
float			f_MotSlaStaSpeed_45S	= 0.0f;
float			f_MotSlaStaSpeed_135S	= 0.0f;
float			f_MotSlaStaSpeed_V90	= 0.0f;
enMOT_WALL_EDGE_TYPE	en_WallEdge = MOT_WALL_EDGE_NONE;	// 壁切れ補正
bool			bl_IsWallEdge = FALSE;				// 壁切れ検知（TRUE:検知、FALSE：非検知）
float			f_WallEdgeAddDist =0.0;				// 壁切れ補正の移動距離

en_TURNTYPE		en_TurnType;



float MOT_getAcc1( void )
{
	return PARAM_getSpeed( PARAM_ST )->f_speed_acc;
}

float MOT_getAcc3( void )
{
	return PARAM_getSpeed( PARAM_ST )->f_speed_dec;
}

float MOT_getJerk( void )
{
	return PARAM_getSpeed( PARAM_ST )->f_speed_jerk;
}

void MOT_goBlock_AccConstDec( float f_fin, enMOT_ST_TYPE en_type, enMOT_GO_ST_TYPE en_goType )
{
	stCTRL_DATA		st_data;					// CTRLdata
	GYRO_staErrChkAngle();

	/* ================ */
	/*      motion      */
	/* ================ */
	/* ------ */
	/*  acc+jerk   */
	/* ------ */
	if( ( en_type != MOT_CONST_DEC ) && ( en_type != MOT_CONST_DEC_CUSTOM ) ){

		if( MOT_GO_ST_NORMAL == en_goType ){
			st_data.en_ctrl_type		= CTRL_ACC;
		}
		else{
			st_data.en_ctrl_type		= CTRL_SKEW_ACC;
		}
		st_data.f_ctrl_jerk			= st_Info.f_mot_jerk;
		st_data.f_ctrl_trgtAcc		= st_Info.f_mot_trgtAcc1;		// 加速度指定
		st_data.f_ctrl_nowAcc		= 0;
		st_data.f_ctrl_now			= st_Info.f_mot_now;		// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_now+st_Info.f_mot_accjerk_v;		// 目標速度
		st_data.f_ctrl_nowDist		= 0;				// 進んでいない
		st_data.f_ctrl_dist			= st_Info.f_mot_l1_accjerk;			// 加速距離
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;				// 現在角速度
		st_data.f_ctrl_trgtAngleS		= 0;				// 目標角度
		st_data.f_ctrl_nowAngle		= 0;				// 現在角度
		st_data.f_ctrl_angle			= 0;				// 目標角度
		st_data.f_ctrl_time 			= 0;				// 目標時間 [sec] ← 指定しない
		CTRL_clrData();								// 設定データをクリア
		CTRL_clrSpeedErrSum();
		CTRL_setData( &st_data );						// データセット
		DCM_staMotAll();							// モータON
		while( Get_NowDist() < st_Info.f_mot_l1_accjerk ){					// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			MOT_setWallEdgeDist();

		}
		//acc
		st_data.f_ctrl_jerk			= 0;
		st_data.f_ctrl_trgtAcc		= st_Info.f_mot_trgtAcc1;		// 加速度指定
		st_data.f_ctrl_nowAcc		= st_Info.f_mot_trgtAcc1;
		st_data.f_ctrl_now			= st_Info.f_mot_now+st_Info.f_mot_accjerk_v;		// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_trgt - st_Info.f_mot_accjerk_v;		// 目標速度
		st_data.f_ctrl_nowDist		= st_Info.f_mot_l1_accjerk;				// 進んでいない
		st_data.f_ctrl_dist			= st_Info.f_mot_l1_accjerk+st_Info.f_mot_l1_accconst;			// 加速距離
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;				// 現在角速度
		st_data.f_ctrl_trgtAngleS		= 0;				// 目標角度
		st_data.f_ctrl_nowAngle		= 0;				// 現在角度
		st_data.f_ctrl_angle			= 0;				// 目標角度
		st_data.f_ctrl_time 			= 0;				// 目標時間 [sec] ← 指定しない
//		CTRL_clrData();								// 設定データをクリア
		CTRL_setData( &st_data );						// データセット
		DCM_staMotAll();							// モータON
		while( Get_NowDist() < (st_Info.f_mot_l1_accjerk + st_Info.f_mot_l1_accconst)){					// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			MOT_setWallEdgeDist();

		}
		//acc - jerk
		st_data.f_ctrl_jerk			= st_Info.f_mot_jerk*(-1.0);
		st_data.f_ctrl_trgtAcc		= 0;		// 加速度指定
		st_data.f_ctrl_nowAcc		= st_Info.f_mot_trgtAcc1;
		st_data.f_ctrl_now			= st_Info.f_mot_trgt-st_Info.f_mot_accjerk_v;		// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_trgt;		// 目標速度
		st_data.f_ctrl_nowDist		= st_Info.f_mot_l1_accjerk + st_Info.f_mot_l1_accconst;				
		st_data.f_ctrl_dist			= st_Info.f_mot_l1;			// 加速距離
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;				// 現在角速度
		st_data.f_ctrl_trgtAngleS		= 0;				// 目標角度
		st_data.f_ctrl_nowAngle		= 0;				// 現在角度
		st_data.f_ctrl_angle			= 0;				// 目標角度
		st_data.f_ctrl_time 			= 0;				// 目標時間 [sec] ← 指定しない
//		CTRL_clrData();								// 設定データをクリア
		CTRL_setData( &st_data );						// データセット
		DCM_staMotAll();							// モータON
		while( Get_NowDist() < st_Info.f_mot_l1 ){					// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			MOT_setWallEdgeDist();

		}

	}

	/* ------ */
	/*  const */
	/* ------ */
	if( MOT_GO_ST_NORMAL == en_goType ){
		st_data.en_ctrl_type		= CTRL_CONST;
	}
	else{
		st_data.en_ctrl_type		= CTRL_SKEW_CONST;
	}
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;		// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_Info.f_mot_trgt;			// 現在速度
	st_data.f_ctrl_trgt			= st_Info.f_mot_trgt;			// 目標速度
	st_data.f_ctrl_nowDist		= st_Info.f_mot_l1;				// 現在位置
	st_data.f_ctrl_dist			= st_Info.f_mot_l1_2;			// 等速完了位置
	st_data.f_ctrl_jerkAngle		= 0;
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
	st_data.f_ctrl_nowAngleS		= 0;					// 現在角速度
	st_data.f_ctrl_trgtAngleS		= 0;					// 目標角度
	st_data.f_ctrl_nowAngle		= 0;					// 現在角度
	st_data.f_ctrl_angle			= 0;					// 目標角度
	st_data.f_ctrl_time 			= 0;					// 目標時間 [sec] ← 指定しない
	if( ( en_type == MOT_CONST_DEC ) || ( en_type == MOT_CONST_DEC_CUSTOM ) ){
		CTRL_clrData();										// 設定データをクリア
	}
	CTRL_setData( &st_data );						// データセット
	while( Get_NowDist() < st_Info.f_mot_l1_2 ){				// 指定距離到達待ち
		if( SYS_isOutOfCtrl() == TRUE ){
			CTRL_stop();
			DCM_brakeMot( DCM_R );		
			DCM_brakeMot( DCM_L );		
			break;
		}				
		MOT_setWallEdgeDist();

	}

	/* ------ */
	/*  dec - jerk  */
	/* ------ */
	if( ( en_type != MOT_ACC_CONST ) && ( en_type != MOT_ACC_CONST_CUSTOM ) ){

		if( MOT_GO_ST_NORMAL == en_goType ){
			st_data.en_ctrl_type		= CTRL_DEC;
		}
		else{
			st_data.en_ctrl_type		= CTRL_SKEW_DEC;
		}
		st_data.f_ctrl_jerk			= st_Info.f_mot_jerk*(-1.0) ;
		st_data.f_ctrl_trgtAcc		= st_Info.f_mot_trgtAcc3*(-1.0);		// 減速
		st_data.f_ctrl_nowAcc		= 0;
		st_data.f_ctrl_now			= st_Info.f_mot_trgt;			// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_trgt - st_Info.f_mot_decjerk_v;			// 最終速度
		st_data.f_ctrl_nowDist		= st_Info.f_mot_l1_2;			// 等速完了位置
		st_data.f_ctrl_dist			= st_Info.f_mot_l1_2 + st_Info.f_mot_l3_decjerk;			// 全移動完了位置
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
		st_data.f_ctrl_trgtAngleS		= 0;						// 目標角度
		st_data.f_ctrl_nowAngle		= 0;						// 現在角度
		st_data.f_ctrl_angle			= 0;						// 目標角度
		st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
		CTRL_setData( &st_data );							// データセット
		while( Get_NowDist() < ( st_Info.f_mot_l1_2 + st_Info.f_mot_l3_decjerk ) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			MOT_setWallEdgeDist();
			

		}

		//dec
		st_data.f_ctrl_jerk			= 0 ;
		st_data.f_ctrl_trgtAcc		= st_Info.f_mot_trgtAcc3*(-1.0);		// 減速
		st_data.f_ctrl_nowAcc		= st_Info.f_mot_trgtAcc3*(-1.0);
		st_data.f_ctrl_now			= st_Info.f_mot_trgt - st_Info.f_mot_decjerk_v;			// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_last + st_Info.f_mot_decjerk_v;			// 最終速度
		st_data.f_ctrl_nowDist		= st_Info.f_mot_l1_2 + st_Info.f_mot_l3_decjerk;			// 等速完了位置
		st_data.f_ctrl_dist			= st_Info.f_mot_l1_2 + st_Info.f_mot_l3_decjerk+st_Info.f_mot_l3_decconst;			// 全移動完了位置
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
		st_data.f_ctrl_trgtAngleS		= 0;						// 目標角度
		st_data.f_ctrl_nowAngle		= 0;						// 現在角度
		st_data.f_ctrl_angle			= 0;						// 目標角度
		st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
		CTRL_setData( &st_data );							// データセット
		while( Get_NowDist() < ( st_Info.f_mot_l1_2 + st_Info.f_mot_l3_decjerk+st_Info.f_mot_l3_decconst ) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			MOT_setWallEdgeDist();

			if((EscapeWait>2.0)&&(SearchFlag == TRUE))break;

		}

		//dec + accjerk
		st_data.f_ctrl_jerk			= st_Info.f_mot_jerk ;
		st_data.f_ctrl_trgtAcc		= 0;		// 減速
		st_data.f_ctrl_nowAcc		= st_Info.f_mot_trgtAcc3*(-1.0);
		st_data.f_ctrl_now			= st_Info.f_mot_last + st_Info.f_mot_decjerk_v;			// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_last;			// 最終速度
		st_data.f_ctrl_nowDist		= st_Info.f_mot_l1_2 + st_Info.f_mot_l3_decjerk+st_Info.f_mot_l3_decconst;			// 等速完了位置
		st_data.f_ctrl_dist			= st_Info.f_mot_dist;			// 全移動完了位置
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
		st_data.f_ctrl_trgtAngleS		= 0;						// 目標角度
		st_data.f_ctrl_nowAngle		= 0;						// 現在角度
		st_data.f_ctrl_angle			= 0;						// 目標角度
		st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
		CTRL_setData( &st_data );							// データセット
		while( Get_NowDist() < ( st_Info.f_mot_dist ) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			MOT_setWallEdgeDist();

			if((EscapeWait>2.0)&&(SearchFlag == TRUE))break;
		}

	}

	/* -------------------- */
	/*  const walledge      */
	/* -------------------- */
	/* not found edge */
	if( ( en_WallEdge != MOT_WALL_EDGE_NONE ) && ( bl_IsWallEdge == FALSE )  ){
		st_data.en_ctrl_type			= CTRL_CONST;
		st_data.f_ctrl_jerk			= 0;
		st_data.f_ctrl_trgtAcc		= 0;		// 加速度指定
		st_data.f_ctrl_nowAcc		= 0;
		st_data.f_ctrl_now			= st_Info.f_mot_last;			// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_last;			// 目標速度
		st_data.f_ctrl_nowDist		= Get_NowDist();				// 現在位置
		st_data.f_ctrl_dist			= Get_NowDist() + 0.045f;	// 等速完了位置（90.0f：壁切れをどこまで救うかの距離）、ここではf_NowDistをクリアしてはいけない。
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
		st_data.f_ctrl_trgtAngleS	= 0;						// 目標角度
		st_data.f_ctrl_nowAngle		= 0;						// 現在角度
		st_data.f_ctrl_angle			= 0;						// 目標角度
		st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
		CTRL_clrData();										// マウスの現在位置/角度をクリア
		CTRL_setData( &st_data );							// データセット
		while( Get_NowDist() < st_data.f_ctrl_dist ){				// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			if( MOT_setWallEdgeDist_LoopWait() == TRUE ) break;	// 壁切れ補正を実行する距離を設定
		}
	}
	/* straight for edge */
	if( ( MOT_GO_ST_NORMAL == en_goType ) &&				// 直進時に追加動作が必要な場合にしか実施しない
		( f_WallEdgeAddDist != 0.0f ) &&
		( f_fin != 0.0f )
	){
		st_data.en_ctrl_type			= CTRL_CONST;
		st_data.f_ctrl_jerk			= 0;
		st_data.f_ctrl_trgtAcc		= 0;		// 加速度指定
		st_data.f_ctrl_nowAcc		= 0;
		st_data.f_ctrl_now			= st_Info.f_mot_last;			// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_last;			// 目標速度
		st_data.f_ctrl_nowDist		= 0;						// 現在位置
		st_data.f_ctrl_dist			= f_WallEdgeAddDist;		// 等速完了位置
		st_data.f_ctrl_jerkAngle		= 0;
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
		st_data.f_ctrl_trgtAngleS	= 0;						// 目標角度
		st_data.f_ctrl_nowAngle		= 0;						// 現在角度
		st_data.f_ctrl_angle			= 0;						// 目標角度
		st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
		CTRL_clrData();										// マウスの現在位置/角度をクリア
		CTRL_setData( &st_data );							// データセット
		while( Get_NowDist() < st_data.f_ctrl_dist ){			// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
		}
	}

	/* stop */
	if( 0.0f == f_fin ){
		LL_mDelay(100);			
	 	CTRL_stop();				
		DCM_brakeMot( DCM_R );	
		DCM_brakeMot( DCM_L );	
	}

	f_MotNowSpeed = f_fin;		
	GYRO_endErrChkAngle();
//	CTRL_clrNowData();
	CTRL_setNowData_Err(/*st_data.f_dist,*/st_data.f_ctrl_angle);
}

void MOT_setData_ACC_CONST_DEC( float f_num, float f_fin, enMOT_GO_ST_TYPE en_type )
{
	float			f_l3;						// 第3移動距離[m]
	float			f_1blockDist;				// 1区画の距離[m]

	float			f_accTime;

	/* 1区画の距離 */
	if( MOT_GO_ST_NORMAL == en_type ){		// 通常の直進
		f_1blockDist = BLOCK;
	}
	else{									// 斜めの直進
		f_1blockDist = BLOCK_SKEW;
	}
	//jerk
	st_Info.f_mot_jerk		= MOT_getJerk();

	/* 加速度 */
	st_Info.f_mot_trgtAcc1 		= MOT_getAcc1();								// 加速度1[mm/s^2]
	st_Info.f_mot_trgtAcc3 		= MOT_getAcc3();								// 加速度3[mm/s^2]

	/* 速度 */
	st_Info.f_mot_now		= f_MotNowSpeed;								// 現在速度
	st_Info.f_mot_trgt		= f_MotTrgtSpeed;								// 目標速度
	st_Info.f_mot_last		= f_fin;									// 最終速度

	/* 距離 */
	st_Info.f_mot_dist		= f_num * f_1blockDist;	

	f_accTime	= st_Info.f_mot_trgtAcc1/st_Info.f_mot_jerk;
	st_Info.f_mot_accjerk_v	= 1.0/2.0*st_Info.f_mot_jerk*f_accTime*f_accTime;//加速度到達時の速度
	st_Info.f_mot_decjerk_v = st_Info.f_mot_accjerk_v;

	printf("1 %f,%f\r\n",st_Info.f_mot_accjerk_v,st_Info.f_mot_decjerk_v);
	printf("1 %f,%f\r\n",f_MotTrgtSpeed,f_MotNowSpeed);
	if(fabsf(f_MotTrgtSpeed - f_MotNowSpeed) > 0.05){
		st_Info.f_mot_l1_accjerk	= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;//加速度に到達するまでの距離											// 移動距離[m]
		st_Info.f_mot_l1_decjerk	= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + (st_Info.f_mot_trgt-st_Info.f_mot_accjerk_v)*f_accTime 
										+ 1.0/2.0*st_Info.f_mot_trgtAcc1*f_accTime*f_accTime;
		st_Info.f_mot_l1_accconst	= ((f_MotTrgtSpeed-st_Info.f_mot_accjerk_v)*(f_MotTrgtSpeed-st_Info.f_mot_accjerk_v)
										-(f_MotNowSpeed+st_Info.f_mot_accjerk_v)*(f_MotNowSpeed+st_Info.f_mot_accjerk_v))
										/( st_Info.f_mot_trgtAcc1 * 2.0 );
	}else{
		st_Info.f_mot_l1_accjerk	= 0.0;//加速度に到達するまでの距離											// 移動距離[m]
		st_Info.f_mot_l1_decjerk	= 0.0;
		st_Info.f_mot_l1_accconst	= 0.0;
		st_Info.f_mot_accjerk_v		= 0.0;
	}
	st_Info.f_mot_l1	= st_Info.f_mot_l1_accjerk + st_Info.f_mot_l1_decjerk + st_Info.f_mot_l1_accconst;
//	st_Info.f_l1		= ( f_MotTrgtSpeed * f_MotTrgtSpeed - f_MotNowSpeed * f_MotNowSpeed ) / ( st_Info.f_acc1 * 2.0 );			// 第1移動距離[m]

	if((f_MotTrgtSpeed - f_fin) != 0){
		st_Info.f_mot_l3_decjerk	= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + st_Info.f_mot_trgt*f_accTime;
		st_Info.f_mot_l3_accjerk	= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + (f_fin+st_Info.f_mot_decjerk_v)*f_accTime 
										+ 1.0/2.0*st_Info.f_mot_trgtAcc3*(-1.0)*f_accTime*f_accTime;//減速は加速度同様にする前提処理
		st_Info.f_mot_l3_decconst	= ( (f_fin+st_Info.f_mot_decjerk_v) * (f_fin+st_Info.f_mot_decjerk_v)
										- (f_MotTrgtSpeed-st_Info.f_mot_decjerk_v) * (f_MotTrgtSpeed-st_Info.f_mot_decjerk_v) ) 
										/ ( st_Info.f_mot_trgtAcc3 * (-1.0 ) * 2.0 );
	}else{
		st_Info.f_mot_l3_decjerk	= 0.0;
		st_Info.f_mot_l3_accjerk	= 0.0;
		st_Info.f_mot_l3_decconst	= 0.0;
		st_Info.f_mot_decjerk_v		= 0.0;
	}
	f_l3				=  st_Info.f_mot_l3_decjerk + st_Info.f_mot_l3_accjerk + st_Info.f_mot_l3_decconst;	
//	f_l3			= ( f_fin * f_fin - f_MotTrgtSpeed * f_MotTrgtSpeed ) / ( ( st_Info.f_acc3 * -1.0 ) * 2.0 );			// 第3移動距離[m]
	st_Info.f_mot_l1_2		= st_Info.f_mot_dist - f_l3;											// 第1+2移動距離[m]

	printf("1 %f,%f\r\n",st_Info.f_mot_accjerk_v,st_Info.f_mot_decjerk_v);
//	printf("1 %f,%f\r",st_Info.f_trgt,st_Info.f_l1);
}

void MOT_setData_MOT_ACC_CONST_DEC_CUSTOM( float f_num, float f_fin, enMOT_GO_ST_TYPE en_type )
{
	float			f_l3;						// 第3移動距離[m]
	float			f_1blockDist;				// 1区画の距離[m]

	float			f_accTime;

	/* 1区画の距離 */
	if( MOT_GO_ST_NORMAL == en_type ){		// 通常の直進
		f_1blockDist = BLOCK;
	}
	else{									// 斜めの直進
		f_1blockDist = BLOCK_SKEW;
	}
	//jerk
	st_Info.f_mot_jerk		= MOT_getJerk();

	/* 加速度 */
	st_Info.f_mot_trgtAcc1 		= MOT_getAcc1();								// 加速度1[mm/s^2]
	st_Info.f_mot_trgtAcc3 		= MOT_getAcc3();								// 加速度3[mm/s^2]


	/* 距離 */
	st_Info.f_mot_dist		= f_num * f_1blockDist;												// 移動距離[m]

	/* 速度 */
	st_Info.f_mot_now		= f_MotNowSpeed;												// 現在速度
	st_Info.f_mot_last		= f_fin;													// 最終速度
/*	st_Info.f_mot_trgt		= sqrt( 1.0 / ( ( st_Info.f_mot_acc3 * -1.0 ) - st_Info.f_mot_acc1 ) *
								( 2.0 * st_Info.f_mot_acc1 * ( st_Info.f_mot_acc3 * -1.0 ) * ( st_Info.f_mot_dist - MOT_MOVE_ST_MIN ) +
								( st_Info.f_mot_acc3 * -1.0 ) * f_MotNowSpeed * f_MotNowSpeed - st_Info.f_mot_acc1 * f_fin * f_fin ) );
*/
/*
	st_Info.f_mot_trgt		= sqrt(1.0/(st_Info.f_mot_trgtAcc1/st_Info.f_mot_trgtAcc3)
									*st_Info.f_mot_jerk*(2.0*( st_Info.f_mot_dist - MOT_MOVE_ST_MIN )
									+(f_MotNowSpeed * f_MotNowSpeed/st_Info.f_mot_trgtAcc1)+(f_fin*f_fin/st_Info.f_mot_trgtAcc3)));
*/
/*
	st_Info.f_mot_l1		= ( st_Info.f_mot_trgt * st_Info.f_mot_trgt - f_MotNowSpeed * f_MotNowSpeed ) / ( st_Info.f_mot_acc1 * 2.0 );			// 第1移動距離[m]
	f_l3					= ( f_fin * f_fin - st_Info.f_mot_trgt * st_Info.f_mot_trgt ) / ( ( st_Info.f_mot_acc3  * -1.0 ) * 2.0 );			// 第3移動距離[m]
	st_Info.f_mot_l1_2		= st_Info.f_mot_dist - f_l3;											// 第1+2移動距離[m]
*/

	f_accTime	= st_Info.f_mot_trgtAcc1/st_Info.f_mot_jerk;

	st_Info.f_mot_accjerk_v		= 1.0/2.0*st_Info.f_mot_jerk*f_accTime*f_accTime;//加速度到達時の速度
	st_Info.f_mot_decjerk_v		= st_Info.f_mot_accjerk_v;
//すべての計算を真面目にやると全然やってられんので最低移動距離を伸ばして余裕作って代用しておくジャーク分を移動距離からサボってる
	st_Info.f_mot_trgt		= sqrt( 1.0 / ( ( st_Info.f_mot_trgtAcc3 * -1.0 ) - st_Info.f_mot_trgtAcc1 ) *
								( 2.0 * st_Info.f_mot_trgtAcc1 * ( st_Info.f_mot_trgtAcc3 * -1.0 ) * 
								( st_Info.f_mot_dist - MOT_MOVE_ST_MIN*3.0) +
								( st_Info.f_mot_trgtAcc3 * -1.0 ) * (f_MotNowSpeed+st_Info.f_mot_accjerk_v) * (f_MotNowSpeed+st_Info.f_mot_accjerk_v)
								 - st_Info.f_mot_trgtAcc1 * (f_fin-st_Info.f_mot_decjerk_v) * (f_fin-st_Info.f_mot_decjerk_v) ) )+st_Info.f_mot_decjerk_v;

	if(fabsf(f_MotTrgtSpeed - f_MotNowSpeed) > 0.05){
		st_Info.f_mot_l1_accjerk	= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;//加速度に到達するまでの距離											// 移動距離[m]
		st_Info.f_mot_l1_decjerk	= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + (st_Info.f_mot_trgt-st_Info.f_mot_accjerk_v)*f_accTime + 1.0/2.0*st_Info.f_mot_trgtAcc1*f_accTime*f_accTime;
		st_Info.f_mot_l1_accconst	= ((st_Info.f_mot_trgt-st_Info.f_mot_accjerk_v)*(st_Info.f_mot_trgt-st_Info.f_mot_accjerk_v)
										-(f_MotNowSpeed+st_Info.f_mot_accjerk_v)*(f_MotNowSpeed+st_Info.f_mot_accjerk_v))
										/( st_Info.f_mot_trgtAcc1 * 2.0 );
	}else{
		st_Info.f_mot_l1_accjerk	= 0.0;//加速度に到達するまでの距離											// 移動距離[m]
		st_Info.f_mot_l1_decjerk	= 0.0;
		st_Info.f_mot_l1_accconst	= 0.0;
		st_Info.f_mot_accjerk_v		= 0.0;
	}
	st_Info.f_mot_l1	= st_Info.f_mot_l1_accjerk + st_Info.f_mot_l1_decjerk + st_Info.f_mot_l1_accconst;

	if((st_Info.f_mot_trgt - f_fin) != 0){
		st_Info.f_mot_l3_decjerk	= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + st_Info.f_mot_trgt*f_accTime;
		st_Info.f_mot_l3_accjerk	= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + (f_fin+st_Info.f_mot_decjerk_v)*f_accTime + 1.0/2.0*st_Info.f_mot_trgtAcc3*(-1.0)*f_accTime*f_accTime;//減速は加速度同様にする前提処理
		st_Info.f_mot_l3_decconst	= ( (f_fin+st_Info.f_mot_decjerk_v) * (f_fin+st_Info.f_mot_decjerk_v)
										- (st_Info.f_mot_trgt-st_Info.f_mot_decjerk_v) * (st_Info.f_mot_trgt-st_Info.f_mot_decjerk_v) ) 
										/ ( st_Info.f_mot_trgtAcc3 * (-1.0 ) * 2.0 );
	}else{
		st_Info.f_mot_l3_decjerk	= 0.0;
		st_Info.f_mot_l3_accjerk	= 0.0;
		st_Info.f_mot_l3_decconst	= 0.0;
		st_Info.f_mot_decjerk_v		= 0.0;
	}
	f_l3				=  st_Info.f_mot_l3_decjerk + st_Info.f_mot_l3_accjerk + st_Info.f_mot_l3_decconst;
	st_Info.f_mot_l1_2		= st_Info.f_mot_dist - f_l3;											// 第1+2移動距離[m]

//	printf("2 %f,%f,%f,%f\r",st_Info.f_trgt,st_Info.f_l1,f_fin,f_MotNowSpeed);
}

void MOT_setData_MOT_ACC_CONST( float f_num, float f_fin, enMOT_GO_ST_TYPE en_type )
{
	float			f_1blockDist;				// 1区画の距離[m]
	float           f_accTime;

	/* 1区画の距離 */
	if( MOT_GO_ST_NORMAL == en_type ){		// 通常の直進
		f_1blockDist = BLOCK;
	}
	else{									// 斜めの直進
		f_1blockDist = BLOCK_SKEW;
	}
	//jerk
	st_Info.f_mot_jerk		= MOT_getJerk();

	/* 加速度 */
	st_Info.f_mot_trgtAcc1 		= MOT_getAcc1();													// 加速度1[mm/s^2]
	st_Info.f_mot_trgtAcc3 		= 0;																// 加速度3[mm/s^2](未使用)

	/* 速度 */
	st_Info.f_mot_now		= f_MotNowSpeed;													// 現在速度
	st_Info.f_mot_trgt		= f_fin;															// 目標速度
	st_Info.f_mot_last		= 0;																// 最終速度(未使用)

	/* 距離 */
	st_Info.f_mot_dist		= f_num * f_1blockDist;												// 移動距離[m]
//	st_Info.f_mot_l1		= ( f_fin * f_fin - f_MotNowSpeed * f_MotNowSpeed ) / ( st_Info.f_mot_acc1 * 2.0 );			// 第1移動距離[m]

	f_accTime   = st_Info.f_mot_trgtAcc1/st_Info.f_mot_jerk;
	st_Info.f_mot_accjerk_v  	= 1.0/2.0*st_Info.f_mot_jerk*f_accTime*f_accTime;//加速度到達時の速度
    if(fabsf(f_MotTrgtSpeed - f_MotNowSpeed) > 0.05){
		st_Info.f_mot_l1_accjerk    = 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;//加速度に到達するまでの距離                                          // 移動距離[m]
		st_Info.f_mot_l1_decjerk    = 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + (st_Info.f_mot_trgt-st_Info.f_mot_accjerk_v)*f_accTime + 1.0/2.0*st_Info.f_mot_trgtAcc1*f_accTime*f_accTime;
		st_Info.f_mot_l1_accconst   = ((f_MotTrgtSpeed-st_Info.f_mot_accjerk_v)*(f_MotTrgtSpeed-st_Info.f_mot_accjerk_v)
										-(f_MotNowSpeed+st_Info.f_mot_accjerk_v)*(f_MotNowSpeed+st_Info.f_mot_accjerk_v))
										/( st_Info.f_mot_trgtAcc1 * 2.0 );
	}else{
		st_Info.f_mot_l1_accjerk    = 0.0;//加速度に到達するまでの距離                                          // 移動距離[m]
		st_Info.f_mot_l1_decjerk    = 0.0;
		st_Info.f_mot_l1_accconst   = 0.0;
		st_Info.f_mot_accjerk_v  	= 0.0;
	}
    st_Info.f_mot_l1    = st_Info.f_mot_l1_accjerk + st_Info.f_mot_l1_decjerk + st_Info.f_mot_l1_accconst;
//  st_Info.f_l1        = ( f_MotTrgtSpeed * f_MotTrgtSpeed - f_MotNowSpeed * f_MotNowSpeed ) / ( st_Info.f_acc1 * 2.0 );           // 第1移動距離[m]
   
    st_Info.f_mot_l1_2  = st_Info.f_mot_dist;                                            // 第1+2移動距離[m]
}

void MOT_setData_MOT_ACC_CONST_CUSTOM( float f_num, float f_fin, enMOT_GO_ST_TYPE en_type )
{
	float			f_1blockDist;				// 1区画の距離[m]
	float           f_accTime;

	/* 1区画の距離 */
	if( MOT_GO_ST_NORMAL == en_type ){		// 通常の直進
		f_1blockDist = BLOCK;
	}
	else{									// 斜めの直進
		f_1blockDist = BLOCK_SKEW;
	}
	//jerk
	st_Info.f_mot_jerk		= MOT_getJerk();

	/* 速度 */
	st_Info.f_mot_now		= f_MotNowSpeed;													// 現在速度
	st_Info.f_mot_trgt		= f_fin;															// 目標速度
	st_Info.f_mot_last		= 0;																// 最終速度(未使用)

	/* 距離 */
	st_Info.f_mot_dist		= f_num * f_1blockDist;												// 移動距離[m]

	/* 加速度 */
//	st_Info.f_mot_acc1 		= ( f_fin * f_fin - f_MotNowSpeed * f_MotNowSpeed ) / ( ( st_Info.f_mot_dist - MOT_MOVE_ST_MIN ) * 2.0 );	// 加速度1[mm/s^2]（強制的に書き換え）

	st_Info.f_mot_trgtAcc1		= sqrt(6.0*st_Info.f_mot_jerk*(( st_Info.f_mot_dist - MOT_MOVE_ST_MIN ) - f_MotNowSpeed*MOT_getAcc1()/st_Info.f_mot_jerk)
								/((MOT_getAcc1()/st_Info.f_mot_jerk)*(MOT_getAcc1()/st_Info.f_mot_jerk)));

	st_Info.f_mot_trgtAcc3 		= 0;																// 加速度3[m/s^2](未使用)

	/* 距離 */
//	st_Info.f_mot_l1		= ( f_fin * f_fin - f_MotNowSpeed * f_MotNowSpeed ) / ( st_Info.f_mot_acc1 * 2.0 );			// 第1移動距離[m]
	f_accTime					= st_Info.f_mot_trgtAcc1/st_Info.f_mot_jerk;

	st_Info.f_mot_accjerk_v		= 1.0/2.0*st_Info.f_mot_jerk*f_accTime*f_accTime;//加速度到達時の速度
	if(fabsf(f_MotTrgtSpeed - f_MotNowSpeed) > 0.05){
		st_Info.f_mot_l1_accjerk	= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;//加速度に到達するまでの距離		// 移動距離[m]
		st_Info.f_mot_l1_decjerk	= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + (st_Info.f_mot_trgt-st_Info.f_mot_accjerk_v)*f_accTime + 1.0/2.0*st_Info.f_mot_trgtAcc1*f_accTime*f_accTime;
		st_Info.f_mot_l1_accconst	= ((f_MotTrgtSpeed-st_Info.f_mot_accjerk_v)*(f_MotTrgtSpeed-st_Info.f_mot_accjerk_v)
										-(f_MotNowSpeed+st_Info.f_mot_accjerk_v)*(f_MotNowSpeed+st_Info.f_mot_accjerk_v))
										/( st_Info.f_mot_trgtAcc1 * 2.0 );
	}else{
		st_Info.f_mot_l1_accjerk    = 0.0;//加速度に到達するまでの距離                                          // 移動距離[m]
		st_Info.f_mot_l1_decjerk    = 0.0;
		st_Info.f_mot_l1_accconst   = 0.0;
		st_Info.f_mot_accjerk_v		= 0.0;
	}
	st_Info.f_mot_l1			= st_Info.f_mot_l1_accjerk + st_Info.f_mot_l1_decjerk + st_Info.f_mot_l1_accconst;

	st_Info.f_mot_l1_2			= st_Info.f_mot_dist;													// 第1+2移動距離[m]
}

void MOT_setData_MOT_CONST_DEC( float f_num, float f_fin, enMOT_GO_ST_TYPE en_type )
{
	float           f_l3;                       // 第3移動距離[m]
	float			f_1blockDist;				// 1区画の距離[mm]
	float           f_accTime;

	/* 1区画の距離 */
	if( MOT_GO_ST_NORMAL == en_type ){		// 通常の直進
		f_1blockDist = BLOCK;
	}
	else{									// 斜めの直進
		f_1blockDist = BLOCK_SKEW;
	}
	//jerk
	st_Info.f_mot_jerk		= MOT_getJerk();

	/* 加速度 */
	st_Info.f_mot_trgtAcc1 		= 0;																// 加速度1[mm/s^2](未使用)
	st_Info.f_mot_trgtAcc3 		= MOT_getAcc3();													// 加速度3[mm/s^2]

	/* 速度 */
	st_Info.f_mot_now		= f_MotNowSpeed;													// 現在速度
	st_Info.f_mot_trgt		= f_MotNowSpeed;													// 目標速度
	st_Info.f_mot_last		= f_fin;															// 最終速度(未使用)

	/* 距離 */
	st_Info.f_mot_dist		= f_num * f_1blockDist;												// 移動距離[m]
	st_Info.f_mot_l1		= 0;																// 第1移動距離[m]
//	st_Info.f_mot_l1_2		= st_Info.f_mot_dist - ( f_fin * f_fin - f_MotNowSpeed * f_MotNowSpeed ) / ( ( st_Info.f_mot_acc3 * -1.0 ) * 2.0 );			// 第1-2移動距離[m]

	f_accTime   = st_Info.f_mot_trgtAcc3/st_Info.f_mot_jerk;
    st_Info.f_mot_decjerk_v  	= 1.0/2.0*st_Info.f_mot_jerk*f_accTime*f_accTime;//加速度到達時の速度
	if((f_MotTrgtSpeed - f_fin) != 0){
		st_Info.f_mot_l3_decjerk    = 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;
		st_Info.f_mot_l3_accjerk    = 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + (f_fin-st_Info.f_mot_decjerk_v)*f_accTime + 1.0/2.0*st_Info.f_mot_trgtAcc3*(-1.0)*f_accTime*f_accTime;//減速は加速度同様にする前提処理
		st_Info.f_mot_l3_decconst   = ( (f_fin+st_Info.f_mot_decjerk_v) * (f_fin+st_Info.f_mot_decjerk_v) 
										- (f_MotTrgtSpeed-st_Info.f_mot_decjerk_v) * (f_MotTrgtSpeed-st_Info.f_mot_decjerk_v) ) 
										/ ( st_Info.f_mot_trgtAcc3 * (-1.0 ) * 2.0 );
	}else{
		st_Info.f_mot_l3_decjerk    = 0.0;
		st_Info.f_mot_l3_accjerk    = 0.0;//減速は加速度同様にする前提処理
		st_Info.f_mot_l3_decconst   = 0.0;
		st_Info.f_mot_decjerk_v  	= 0.0;
	}
    f_l3                	= st_Info.f_mot_l3_decjerk + st_Info.f_mot_l3_accjerk + st_Info.f_mot_l3_decconst;    
    st_Info.f_mot_l1_2      = st_Info.f_mot_dist - f_l3;                                            // 第1+2移動距離[m]
}

void MOT_setData_MOT_CONST_DEC_CUSTOM( float f_num, float f_fin, enMOT_GO_ST_TYPE en_type )
{
	float			f_1blockDist;				// 1区画の距離[m]
	float			f_accTime;

	/* 1区画の距離 */
	if( MOT_GO_ST_NORMAL == en_type ){		// 通常の直進
		f_1blockDist = BLOCK;
	}
	else{									// 斜めの直進
		f_1blockDist = BLOCK_SKEW;
	}
	//jerk
	st_Info.f_mot_jerk		= MOT_getJerk();

	/* 速度 */
	st_Info.f_mot_now		= f_MotNowSpeed;									// 現在速度
	st_Info.f_mot_trgt		= f_MotNowSpeed;									// 目標速度
	st_Info.f_mot_last		= f_fin;															// 最終速度

	/* 距離 */
	st_Info.f_mot_dist		= f_num * f_1blockDist;									// 移動距離[mm]

	/* 加速度 */
	st_Info.f_mot_trgtAcc1 		= 0;																// 加速度1[mm/s^2](未使用)
//	st_Info.f_mot_acc3 		= ( f_fin * f_fin - f_MotNowSpeed * f_MotNowSpeed ) / ( ( st_Info.f_mot_dist - MOT_MOVE_ST_MIN ) * 2.0 ) * -1.0;	// 加速度3[mm/s^2]（強制的に書き換え）
	st_Info.f_mot_trgtAcc3		= sqrt(6.0*st_Info.f_mot_jerk*(( st_Info.f_mot_dist - MOT_MOVE_ST_MIN ) - f_MotNowSpeed*MOT_getAcc3()/st_Info.f_mot_jerk)
								/((MOT_getAcc3()/st_Info.f_mot_jerk)*(MOT_getAcc3()/st_Info.f_mot_jerk)));

	/* 距離 */
	st_Info.f_mot_l1		= 0;																// 第1移動距離[m]

	f_accTime					= st_Info.f_mot_trgtAcc3/st_Info.f_mot_jerk;

	st_Info.f_mot_decjerk_v		= 1.0/2.0*st_Info.f_mot_jerk*f_accTime*f_accTime;//加速度到達時の速度
	if((f_MotNowSpeed- f_fin)!=0){
		st_Info.f_mot_l3_decjerk    = 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;
		st_Info.f_mot_l3_accjerk    = 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + (f_fin+st_Info.f_mot_decjerk_v)*f_accTime + 1.0/2.0*st_Info.f_mot_trgtAcc3*(-1.0)*f_accTime*f_accTime;//減速は加速度同様にする前提処理
		st_Info.f_mot_l3_decconst	= ((f_fin-st_Info.f_mot_decjerk_v)*(f_fin-st_Info.f_mot_decjerk_v)
										-(f_MotNowSpeed+st_Info.f_mot_decjerk_v)*(f_MotNowSpeed+st_Info.f_mot_decjerk_v))
										/( st_Info.f_mot_trgtAcc3 * (-1.0) * 2.0 );
	}else{
		st_Info.f_mot_l3_decjerk    = 0.0;
		st_Info.f_mot_l3_accjerk    = 0.0;//減速は加速度同様にする前提処理
		st_Info.f_mot_l3_decconst	= 0.0;
	}
//	st_Info.f_mot_l1			= st_Info.f_mot_l3_accjerk*2.0 + st_Info.f_mot_l3_accconst;


	st_Info.f_mot_l1_2		= st_Info.f_mot_dist - (st_Info.f_mot_l3_decjerk + st_Info.f_mot_l3_accjerk + st_Info.f_mot_l3_decconst);			// 第1-2移動距離[m]
}

enMOT_ST_TYPE MOT_getStType( float f_num, float f_fin, enMOT_GO_ST_TYPE en_type )
{
	float f_v1Div;
	float f_v3Div;
	float f_acc1;
	float f_acc3;
	float f_t1;
	float f_t3;
	float f_l1;							//加速距離
	float f_l3;							//減速距離
	float f_total;							// 移動距離[m]

	float f_Jerk;
	float f_accTime;
	float f_l_acc_accJerk;
	float f_l_acc_accConst;
	float f_l_acc_decJerk;
	float f_v_accJerk;

	/* 1区画の距離 */
	if( MOT_GO_ST_NORMAL == en_type ){		// 通常の直進
		f_total	= f_num * BLOCK;
	}
	else{									// 斜めの直進
		f_total	= f_num * BLOCK_SKEW;
	}

	f_Jerk		= MOT_getJerk();
	f_acc1		= MOT_getAcc1();
	f_acc3		= MOT_getAcc3();

	/* ================ */
	/*   加速＋等速動作  */
	/* ================ */
//	f_v1Div		= f_fin - f_MotNowSpeed;
//	f_acc1		= MOT_getAcc1();				// 加速度1[mm/s^2]
//	f_t1		= f_v1Div / f_acc1;

//	f_l1 = ( f_MotNowSpeed + f_fin ) * 0.5f * f_t1;

	f_accTime			= f_acc1/f_Jerk;
	f_v_accJerk			= 1.0/2.0*f_Jerk*f_accTime*f_accTime;
	f_l_acc_accJerk		= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;
	f_l_acc_decJerk		= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + (f_fin-f_v_accJerk)*f_accTime + 1.0/2.0*f_acc1*f_accTime*f_accTime;
	f_l_acc_accConst	= ((f_fin-f_v_accJerk)*(f_fin-f_v_accJerk)
							-(f_MotNowSpeed+f_v_accJerk)*(f_MotNowSpeed+f_v_accJerk))
							/( f_acc1 * 2.0 );
	f_l1	= f_l_acc_accJerk + f_l_acc_decJerk + f_l_acc_accConst;

//	printf("1, %f ,2, %f ,3, %f\n\r",f_l_acc_accJerk,f_l_acc_accConst,f_l_acc_decJerk);
//	printf("l, %f\r\n",f_l1);
	/*  加速＋等速動作 */
	if( f_total <= ( f_l1 + MOT_MOVE_ST_THRESHOLD ) ){

		/* 加速が最終速度に対して完了しない */
		if( f_total < ( f_l1 + MOT_MOVE_ST_MIN ) ){
//			printf("パターン4\n\r");
			return MOT_ACC_CONST_CUSTOM;		// パターン4（強制的に加速度を変更する）
		}
		else{
//			printf("パターン3\n\r");
			return MOT_ACC_CONST;				// パターン3（加速＋等速）
		}
	}

	/* ================ */
	/*  等速＋減速動作   */
	/* ================ */
//	f_v3Div		= f_fin - f_MotNowSpeed;
//	f_acc3		= MOT_getAcc3();				// 加速度3[mm/s^2]
//	f_t3		= f_v3Div / ( f_acc3 * -1.0 );

//	f_l3 = ( f_MotNowSpeed + f_fin ) * 0.5f * f_t3;

	f_accTime			= f_acc3/f_Jerk;
	f_v_accJerk			= 1.0/2.0*f_Jerk*f_accTime*f_accTime;
	f_l_acc_decJerk		= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;
	f_l_acc_accJerk		= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + (f_fin+f_v_accJerk)*f_accTime+ 1.0/2.0*f_acc3*(-1.0)*f_accTime*f_accTime;
	f_l_acc_accConst	= ((f_fin-f_v_accJerk)*(f_fin-f_v_accJerk)
							-(f_MotNowSpeed+f_v_accJerk)*(f_MotNowSpeed+f_v_accJerk))
							/( f_acc3 * 2.0 *(-1.0));
	
	f_l3	= f_l_acc_decJerk + f_l_acc_accJerk + f_l_acc_accConst;

//	printf("1, %f ,2, %f ,3, %f\n\r",f_l_acc_accJerk,f_l_acc_accConst,f_l_acc_decJerk);
//	printf("l, %f\r\n",f_l3);
	/* 等速＋減速動作 */
	if( f_total <= ( f_l3 + MOT_MOVE_ST_THRESHOLD ) ){

		/* 減速が最終速度に対して完了しない */
		if( f_total < ( f_l3 + MOT_MOVE_ST_MIN ) ){
//			printf("パターン6\n\r");
			return MOT_CONST_DEC_CUSTOM;		// パターン6（強制的に加速度を変更する）
		}
		else{
//			printf("パターン5\n\r");
			return MOT_CONST_DEC;				// パターン5（等速＋減速）
		}
	}

	/* ========== */
	/*  台形動作  */
	/* ========== */
/*	f_v1Div		= f_MotTrgtSpeed - f_MotNowSpeed;					// 台形時の速度差
	f_t1		= f_v1Div / f_acc1;
	f_l1		= ( f_MotNowSpeed + f_MotTrgtSpeed ) * 0.5f * f_t1;

	f_v3Div		= f_fin - f_MotTrgtSpeed;							// 台形時の速度差
	f_acc3		= MOT_getAcc3();									// 加速度3[mm/s^2]
	f_t3		= -1.0f * f_v3Div / f_acc3;							// 減速時の所要時間
	f_l3		= ( f_MotTrgtSpeed + f_fin ) * 0.5f * f_t3;
*/

	f_accTime			= f_acc1/f_Jerk;
	f_v_accJerk			= 1.0/2.0*f_Jerk*f_accTime*f_accTime;
	f_l_acc_accJerk		= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + f_MotNowSpeed*f_accTime;
	f_l_acc_decJerk		= 1.0/6.0*st_Info.f_mot_jerk*f_accTime*f_accTime*f_accTime + (f_MotTrgtSpeed-f_v_accJerk)*f_accTime + 1.0/2.0*f_acc1*f_accTime*f_accTime;
	f_l_acc_accConst	= ((f_MotTrgtSpeed-f_v_accJerk)*(f_MotTrgtSpeed-f_v_accJerk)
							-(f_MotNowSpeed+f_v_accJerk)*(f_MotNowSpeed+f_v_accJerk))
							/( f_acc1 * 2.0 );

	f_l1	= f_l_acc_accJerk + f_l_acc_decJerk + f_l_acc_accConst;

//	printf("1, %f ,2, %f ,3, %f\n\r",f_l_acc_accJerk,f_l_acc_accConst,f_l_acc_decJerk);
//	printf("l, %f\r\n",f_l1);

	f_accTime			= f_acc3/f_Jerk;
	f_v_accJerk			= 1.0/2.0*f_Jerk*f_accTime*f_accTime;
	f_l_acc_decJerk		= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + f_MotTrgtSpeed*f_accTime;
	f_l_acc_accJerk		= 1.0/6.0*st_Info.f_mot_jerk*(-1.0)*f_accTime*f_accTime*f_accTime + (f_fin+f_v_accJerk)*f_accTime + 1.0/2.0*f_acc3*(-1.0)*f_accTime*f_accTime;
	f_l_acc_accConst	= ((f_fin-f_v_accJerk)*(f_fin-f_v_accJerk)
							-(f_MotTrgtSpeed+f_v_accJerk)*(f_MotTrgtSpeed+f_v_accJerk))
							/( f_acc3 * 2.0 * (-1.0));
	
	f_l3	= f_l_acc_decJerk + f_l_acc_accJerk + f_l_acc_accConst;

//	printf("1, %f ,2, %f ,3, %f\n\r",f_l_acc_accJerk,f_l_acc_accConst,f_l_acc_decJerk);
//	printf("l, %f\r\n",f_l3);

	/* 通常の台形動作 */
	if( ( f_total - f_l1 - f_l3 - MOT_MOVE_ST_MIN) >= 0 ){
//		printf("パターン1\n\r");
		return MOT_ACC_CONST_DEC;				// パターン1（通常）
	}
	/* 等速値を変更する */
	else{
//		printf("パターン2\n\r");
		return MOT_ACC_CONST_DEC_CUSTOM;		// パターン2（目標速度を変更）
	}
}

void MOT_go_FinSpeed( float f_num, float f_fin, enMOT_GO_ST_TYPE en_goStType )
{
	enMOT_ST_TYPE 		en_type 		= MOT_getStType( f_num, f_fin, en_goStType);			// 動作パターン取得
	/* 移動距離と指定値に応じで動作を変える */
	switch( en_type ){

		case MOT_ACC_CONST_DEC:				// [01] 台形加速
			MOT_setData_ACC_CONST_DEC( f_num, f_fin, en_goStType );					// 動作データ作成
			MOT_goBlock_AccConstDec( f_fin, en_type, en_goStType );					// 動作
			break;

		case MOT_ACC_CONST_DEC_CUSTOM:		// [02] 台形加速（等速）
			MOT_setData_MOT_ACC_CONST_DEC_CUSTOM( f_num, f_fin, en_goStType );		// 動作データ作成
			MOT_goBlock_AccConstDec( f_fin, en_type, en_goStType );					// 動作
			break;

		case MOT_ACC_CONST:				// [03] 加速＋等速
			MOT_setData_MOT_ACC_CONST( f_num, f_fin, en_goStType );					// 動作データ作成
			MOT_goBlock_AccConstDec( f_fin, en_type, en_goStType );					// 動作
			break;

		case MOT_ACC_CONST_CUSTOM:		// [04] 加速＋等速（等速）
			MOT_setData_MOT_ACC_CONST_CUSTOM( f_num, f_fin, en_goStType );			// 動作データ作成
			MOT_goBlock_AccConstDec( f_fin, en_type, MOT_GO_ST_NORMAL );			// 動作
			break;

		case MOT_CONST_DEC:				// [05] 等速＋減速
			MOT_setData_MOT_CONST_DEC( f_num, f_fin, en_goStType );					// 動作データ作成
			MOT_goBlock_AccConstDec( f_fin, en_type, en_goStType );					// 動作
			break;

		case MOT_CONST_DEC_CUSTOM:		// [06] 等速＋減速（減速値変更）
			MOT_setData_MOT_CONST_DEC_CUSTOM( f_num, f_fin, en_goStType );			// 動作データ作成
			MOT_goBlock_AccConstDec( f_fin, en_type, en_goStType );					// 動作
			break;

		default:
			break;
	}

}

void MOT_goBlock_FinSpeed( float f_num, float f_fin )
{
	MOT_go_FinSpeed( f_num, f_fin, MOT_GO_ST_NORMAL );		// 通常の直進
}

void MOT_goSkewBlock_FinSpeed( float f_num, float f_fin )
{
	MOT_go_FinSpeed( f_num, f_fin, MOT_GO_ST_SKEW );		// 通常の直進
}

void MOT_goBlock_Const(float f_num)
{
	stCTRL_DATA		st_data;
	stMOT_DATA		st_info;

	GYRO_staErrChkAngle();

	/* ---------------- */
	/*  動作データ計算   */
	/* ---------------- */
	/* 距離 */
	st_info.f_mot_dist		= f_num * BLOCK;													// 移動距離[m]


	/* ------ */
	/*  等速  */
	/* ------ */
	st_data.en_ctrl_type			= CTRL_CONST;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;		// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= f_MotNowSpeed;			// 現在速度
	st_data.f_ctrl_trgt			= f_MotNowSpeed;			// 目標速度
	st_data.f_ctrl_nowDist		= 0;				// 現在位置
	st_data.f_ctrl_dist			= st_info.f_mot_dist;			// 等速完了位置
	st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
	st_data.f_ctrl_nowAngleS		= 0;					// 現在角速度
	st_data.f_ctrl_trgtAngleS		= 0;					// 目標角度
	st_data.f_ctrl_nowAngle		= 0;					// 現在角度
	st_data.f_ctrl_angle			= 0;					// 目標角度
	st_data.f_ctrl_time 			= 0;					// 目標時間 [sec] ← 指定しない
	CTRL_clrData();										// 設定データをクリア
	CTRL_setData( &st_data );						// データセット
	Set_TrgtSpeed(f_MotNowSpeed);
//	printf("目標速度 %f 目標位置 %f \r\n",st_data.f_trgt,st_data.f_dist);
	while( Get_NowDist() < st_info.f_mot_dist ){				// 指定距離到達待ち
		if( SYS_isOutOfCtrl() == TRUE ){
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			break;
		}				// 途中で制御不能になった
		if(MOT_setWallEdgeDist()==TRUE) break;
		if(Is_Known_Accel() == FALSE){
			if((DIST_getNowVal(DIST_SEN_R_FRONT)>(R_FRONT_REF-FRONT_WALL_MISS_DIF))||(DIST_getNowVal(DIST_SEN_L_FRONT)>(L_FRONT_REF-FRONT_WALL_MISS_DIF))){
				front_wall_miss = TRUE;
				break;
			}
		}
	}

	if( ( en_WallEdge != MOT_WALL_EDGE_NONE ) && ( bl_IsWallEdge == FALSE )  ){
		st_data.en_ctrl_type			= CTRL_CONST;
		st_data.f_ctrl_jerk			= 0;
		st_data.f_ctrl_trgtAcc		= 0;		// 加速度指定
		st_data.f_ctrl_nowAcc		= 0;
		st_data.f_ctrl_now			= st_Info.f_mot_last;			// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_last;			// 目標速度
		st_data.f_ctrl_nowDist		= Get_NowDist();				// 現在位置
		st_data.f_ctrl_dist			= Get_NowDist() + 0.045f;	// 等速完了位置（90.0f：壁切れをどこまで救うかの距離）、ここではf_NowDistをクリアしてはいけない。
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
		st_data.f_ctrl_trgtAngleS	= 0;						// 目標角度
		st_data.f_ctrl_nowAngle		= 0;						// 現在角度
		st_data.f_ctrl_angle			= 0;						// 目標角度
		st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
		CTRL_clrData();										// マウスの現在位置/角度をクリア
		CTRL_setData( &st_data );							// データセット
		while( Get_NowDist() < st_data.f_ctrl_dist ){				// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}				
			if( MOT_setWallEdgeDist_LoopWait() == TRUE ) break;	// 壁切れ補正を実行する距離を設定
			if(Is_Known_Accel() == FALSE){
				if((DIST_getNowVal(DIST_SEN_R_FRONT)>(R_FRONT_REF-FRONT_WALL_MISS_DIF))||(DIST_getNowVal(DIST_SEN_L_FRONT)>(L_FRONT_REF-FRONT_WALL_MISS_DIF))){
					front_wall_miss = TRUE;
					break;
				}
			}
		}
	}
	/* straight for edge */
	if( f_WallEdgeAddDist != 0.0f)
	{
		st_data.en_ctrl_type			= CTRL_CONST;
		st_data.f_ctrl_jerk			= 0;
		st_data.f_ctrl_trgtAcc		= 0;		// 加速度指定
		st_data.f_ctrl_nowAcc		= 0;
		st_data.f_ctrl_now			= st_Info.f_mot_last;			// 現在速度
		st_data.f_ctrl_trgt			= st_Info.f_mot_last;			// 目標速度
		st_data.f_ctrl_nowDist		= 0;						// 現在位置
		st_data.f_ctrl_dist			= f_WallEdgeAddDist;		// 等速完了位置
		st_data.f_ctrl_nowAccAngle		= 0;
		st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
		st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
		st_data.f_ctrl_trgtAngleS	= 0;						// 目標角度
		st_data.f_ctrl_nowAngle		= 0;						// 現在角度
		st_data.f_ctrl_angle			= 0;						// 目標角度
		st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
		CTRL_clrData();										// マウスの現在位置/角度をクリア
		CTRL_setData( &st_data );							// データセット
		while( Get_NowDist() < st_data.f_ctrl_dist ){			// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		
				DCM_brakeMot( DCM_L );		
				break;
			}	
			if(Is_Known_Accel() == FALSE){
				if((DIST_getNowVal(DIST_SEN_R_FRONT)>(R_FRONT_REF-FRONT_WALL_MISS_DIF))||(DIST_getNowVal(DIST_SEN_L_FRONT)>(L_FRONT_REF-FRONT_WALL_MISS_DIF))){
					front_wall_miss = TRUE;
					break;
				}
			}			
		}
	}

	if(front_wall_miss == TRUE){
		MOT_goBlock_FinSpeed(0.4,0);
		DCM_brakeMot( DCM_R );		
		DCM_brakeMot( DCM_L );
		LL_mDelay(200);

	}

	MOT_setWallEdgeType( MOT_WALL_EDGE_NONE );		// 壁切れ補正終了
	GYRO_endErrChkAngle();
//	CTRL_clrNowData();
	CTRL_setNowData_Err(/*st_data.f_dist,*/st_data.f_ctrl_angle);
}

void testrun(void)
{
	stCTRL_DATA test;
		test.en_ctrl_type=CTRL_ACC;
		test.f_ctrl_trgtAcc		= MOT_getAcc1();		// 加速度指定
		test.f_ctrl_nowAcc		= 0;
		test.f_ctrl_trgt	= 0;
		test.f_ctrl_now = 0;
		test.f_ctrl_nowDist = 0;
		test.f_ctrl_dist = 0;

	CTRL_clrNowData();
	CTRL_clrData();
	CTRL_setData(&test);
}

float MOT_getAccAngle1( void )
{
	return PARAM_getSpeed( PARAM_TRUN )->f_speed_accangle;
}

float MOT_getAccAngle3( void )
{
	return PARAM_getSpeed( PARAM_TRUN )->f_speed_decangle;
}

float MOT_getJerkAngle( void )
{
	return PARAM_getSpeed( PARAM_TRUN )->f_speed_jerkangle;
}

void MOT_turn( enMOT_TURN_CMD en_type )
{
	stMOT_DATA	st_info;	//シーケンスデータ
	stCTRL_DATA	st_data;	//制御データ
//	float		f_angle2 = A2_MIN;	//最低第2移動角度[rad]
	float		f_angle1;	//第1移動角度[rad]
	float		f_angle3;	//第3移動角度[rad]
	float		us_trgtAngleS;	//目標角度[rad/s]

	float		f_accAngleTime1;
	float		f_accAngleTime3;
	float		f_mot_l1_accanglejerk;
	float		f_mot_l1_decanglejerk;
	float		f_mot_accAnglejerk_v;
    float		f_mot_l1_accangleconst;

	float		f_mot_l3_decanglejerk;
	float		f_mot_l3_accanglejerk;
    float		f_mot_l3_decangleconst;

	us_trgtAngleS = 2.8*PI;//500;
	/* ---------------- */
	/*  動作データ計計算  */
	/* ---------------- */
	st_info.f_mot_jerkAngle = MOT_getJerkAngle();
	/* 加速度 */
	st_info.f_mot_trgtAccAngle1= MOT_getAccAngle1();												// 角加速度1[rad/s^2]
	st_info.f_mot_trgtAccAngle3= MOT_getAccAngle3();												// 角加速度3[rad/s^2]

	/* 角速度 */
	st_info.f_mot_nowAngleS	= 0;																// 現在角速度
	st_info.f_mot_trgtAngleS= (float)us_trgtAngleS;												// 目標角速度
	st_info.f_mot_lastAngleS= 0;																// 最終角速度

	/* 角度 */
	switch( en_type ){
		case MOT_R90:	st_info.f_mot_angle = -PI/2.0 - ANGLE_OFFSET1_R;	break;					// 回転角度[rad]
		case MOT_L90:	st_info.f_mot_angle =  PI/2.0 + ANGLE_OFFSET1;		break;					// 回転角度[rad]
		case MOT_R180:	st_info.f_mot_angle = -PI - ANGLE_OFFSET2_R;	break;					// 回転角度[rad]
		case MOT_L180:	st_info.f_mot_angle =  PI + ANGLE_OFFSET2;		break;					// 回転角度[rad]
		case MOT_R360:	st_info.f_mot_angle = -2.0*PI - ANGLE_OFFSET3;		break;					// 回転角度[rad]
		case MOT_L360:	st_info.f_mot_angle =  2.0*PI + ANGLE_OFFSET3;		break;					// 回転角度[rad]
		default:
			printf("error\r\n");
			break;
	}

	f_accAngleTime1 = st_info.f_mot_trgtAccAngle1/st_info.f_mot_jerkAngle;
	f_accAngleTime3 = st_info.f_mot_trgtAccAngle3/st_info.f_mot_jerkAngle;

	f_mot_accAnglejerk_v	= 1.0/2.0*st_info.f_mot_jerkAngle*f_accAngleTime1*f_accAngleTime1;//加速度到達時の速度
	f_mot_l1_accanglejerk	= 1.0/6.0*st_info.f_mot_jerkAngle*f_accAngleTime1*f_accAngleTime1*f_accAngleTime1;
	f_mot_l1_decanglejerk	= 1.0/6.0*st_info.f_mot_jerkAngle*(-1.0)*f_accAngleTime1*f_accAngleTime1*f_accAngleTime1 + (st_info.f_mot_trgtAngleS-f_mot_accAnglejerk_v)*f_accAngleTime1 + 1.0/2.0* st_info.f_mot_trgtAccAngle1 *f_accAngleTime1*f_accAngleTime1;
    f_mot_l1_accangleconst		= ((us_trgtAngleS-f_mot_accAnglejerk_v)*(us_trgtAngleS-f_mot_accAnglejerk_v)
                                -(0.0+f_mot_accAnglejerk_v)*(0.0+f_mot_accAnglejerk_v))
                                /( st_info.f_mot_trgtAccAngle1 * 2.0 );

	f_angle1 = f_mot_l1_accanglejerk+f_mot_l1_decanglejerk+f_mot_l1_accangleconst;

	f_mot_accAnglejerk_v	= 1.0/2.0*st_info.f_mot_jerkAngle*f_accAngleTime3*f_accAngleTime3;//加速度到達時の速度
	f_mot_l3_decanglejerk	= 1.0/6.0*st_info.f_mot_jerkAngle*(-1.0)*f_accAngleTime3*f_accAngleTime3*f_accAngleTime3 + st_info.f_mot_trgtAngleS*f_accAngleTime3;
	f_mot_l3_accanglejerk	= 1.0/6.0*st_info.f_mot_jerkAngle*f_accAngleTime3*f_accAngleTime3*f_accAngleTime3 + f_mot_accAnglejerk_v*f_accAngleTime3 + 1.0/2.0* st_info.f_mot_trgtAccAngle3*(-1.0) *f_accAngleTime1*f_accAngleTime1;
    f_mot_l3_decangleconst		= ((0.0+f_mot_accAnglejerk_v)*(0.0+f_mot_accAnglejerk_v)
                                -(us_trgtAngleS-f_mot_accAnglejerk_v)*(us_trgtAngleS-f_mot_accAnglejerk_v))
                                /( st_info.f_mot_trgtAccAngle3 *(-1.0)* 2.0 );

	f_angle3 = f_mot_l3_decanglejerk+f_mot_l3_accanglejerk+f_mot_l3_decangleconst;

//	f_angle3 = ( st_info.f_mot_trgtAngleS - st_info.f_mot_lastAngleS ) / 2.0 * ( st_info.f_mot_trgtAngleS - st_info.f_mot_lastAngleS ) / st_info.f_mot_accAngleS3;						// 第3移動角度[rad]
//	f_angle1 = ( 0.0 - st_info.f_mot_trgtAngleS) / 2.0 * ( 0.0 - st_info.f_mot_trgtAngleS ) / st_info.f_mot_accAngleS1;

	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		st_info.f_mot_jerkAngle *= -1.0;
		st_info.f_mot_trgtAngleS*= -1.0;															// 回転方向を逆にする
		f_angle1			*= -1.0;
//		f_angle2 			*= -1;															// 回転方向を逆にする
		f_angle3 			*= -1.0;															// 回転方向を逆にする

		st_info.f_mot_trgtAccAngle1*= -1.0;												// 角加速度1[rad/s^2]
		st_info.f_mot_trgtAccAngle3*= -1.0;

		st_info.f_mot_l1_accanglejerk	= f_mot_l1_accanglejerk	* (-1.0);
		st_info.f_mot_l1_decanglejerk	= f_mot_l1_decanglejerk	* (-1.0);
		st_info.f_mot_l1_accangleconst		= f_mot_l1_accangleconst		* (-1.0);
		st_info.f_mot_accAnglejerk_v = f_mot_accAnglejerk_v * (-1.0);

		st_info.f_mot_l3_decanglejerk	= f_mot_l3_decanglejerk	* (-1.0);
		st_info.f_mot_l3_accanglejerk	= f_mot_l3_accanglejerk	* (-1.0);
    	st_info.f_mot_l3_decangleconst		= f_mot_l3_decangleconst		* (-1.0);

		st_info.f_mot_angle1	= f_angle1;						// 第1移動角度[rad]
		st_info.f_mot_angle1_2	= st_info.f_mot_angle - f_angle3;									// 第1+2移動角度[rad]
		en_TurnType			= Right;

		/* 最小移動距離を上書き */
//		if( st_info.f_mot_angle1 > ( A1_MIN * -1.0 ) ){
//			st_info.f_mot_angle1 = A1_MIN * -1.0;
//		}
	}
	else{
		st_info.f_mot_l1_accanglejerk	= f_mot_l1_accanglejerk;
		st_info.f_mot_l1_decanglejerk	= f_mot_l1_decanglejerk;
		st_info.f_mot_l1_accangleconst		= f_mot_l1_accangleconst;
		st_info.f_mot_accAnglejerk_v = f_mot_accAnglejerk_v;

		st_info.f_mot_l3_decanglejerk	= f_mot_l3_decanglejerk;
		st_info.f_mot_l3_accanglejerk	= f_mot_l3_accanglejerk;
    	st_info.f_mot_l3_decangleconst		= f_mot_l3_decangleconst;

		st_info.f_mot_angle1	= f_angle1;						// 第1移動角度[rad]
		st_info.f_mot_angle1_2	= st_info.f_mot_angle - f_angle3;									// 第1+2移動角度[rad]
		en_TurnType			= Left;

		/* 最小移動距離を上書き */
//		if( st_info.f_mot_angle1 < A1_MIN ){
//			st_info.f_mot_angle1 = A1_MIN;
//		}
	}


	GYRO_staErrChkAngle();			// エラー検出開始
//	printf("目標角度 %f %f %f\r\n",st_info.f_angle,st_info.f_angle1,st_info.f_angle1_2);
	/* ================ */
	/*      実動作       */
	/* ================ */
	/* ------ */
	/*  加速  */
	/* ------ */
	st_data.en_ctrl_type			= CTRL_ACC_TRUN;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 目標速度
	st_data.f_ctrl_nowDist		= 0;						// 進んでいない
	st_data.f_ctrl_dist			= 0;						// 加速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS1;		// 角加速度
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle;
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle1;				// 角加速度
	st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_accAnglejerk_v;		// 目標角度
	st_data.f_ctrl_nowAngle			= 0;						// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_l1_accanglejerk;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_clrData();										// マウスの現在位置/角度をクリア
	CTRL_setData( &st_data );							// データセット
	DCM_staMotAll();									// モータON

	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		while( Get_NowAngle() > st_info.f_mot_l1_accanglejerk ){			// 指定角度到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( Get_NowAngle() < st_info.f_mot_l1_accanglejerk ){			// 指定角度到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	//jerk const
	st_data.en_ctrl_type		= CTRL_ACC_TRUN;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 目標速度
	st_data.f_ctrl_nowDist		= 0;						// 進んでいない
	st_data.f_ctrl_dist			= 0;						// 加速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS1;		// 角加速度
	st_data.f_ctrl_jerkAngle	= 0;
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle1;
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle1;				// 角加速度
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_accAnglejerk_v;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_trgtAngleS-st_info.f_mot_accAnglejerk_v;		// 目標角度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_l1_accanglejerk;						// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_setData( &st_data );							// データセット
	DCM_staMotAll();									// モータON

	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		while( Get_NowAngle() > (st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst) ){			// 指定角度到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( Get_NowAngle() < (st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst) ){			// 指定角度到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	//-jerk
	st_data.en_ctrl_type			= CTRL_ACC_TRUN;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 目標速度
	st_data.f_ctrl_nowDist		= 0;						// 進んでいない
	st_data.f_ctrl_dist			= 0;						// 加速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS1;		// 角加速度
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle*(-1.0);
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle1;
	st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS - st_info.f_mot_accAnglejerk_v;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_trgtAngleS;		
	st_data.f_ctrl_nowAngle			= st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst;						// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle1;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_setData( &st_data );							// データセット
	DCM_staMotAll();									// モータON

	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		while( Get_NowAngle() > st_info.f_mot_angle1 ){			// 指定角度到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( Get_NowAngle() < st_info.f_mot_angle1 ){			// 指定角度到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}

	/* ------ */
	/*  等速  */
	/* ------ */
/*	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		f_angle3			= ( Get_TrgtAngleS() - st_info.f_mot_lastAngleS ) / 2.0 * ( Get_TrgtAngleS() - st_info.f_mot_lastAngleS ) / st_info.f_mot_trgtAccAngle3;		// 第3移動角度[rad]
		f_angle3			= -1.0 * f_angle3;
		if( f_angle3 > A3_MIN*-1.0 ) f_angle3 = A3_MIN * -1.0;																	// 減速最低角度に書き換え
		st_info.f_mot_angle1_2		= st_info.f_mot_angle - f_angle3;// 第1+2移動角度[rad]

	}
	else{
		f_angle3			= ( Get_TrgtAngleS() - st_info.f_mot_lastAngleS ) / 2.0 * ( Get_TrgtAngleS() - st_info.f_mot_lastAngleS ) / st_info.f_mot_trgtAccAngle3;		// 第3移動角度[rad]
		if( f_angle3 < A3_MIN ) f_angle3 = A3_MIN;																			// 減速最低角度に書き換え
		st_info.f_mot_angle1_2		= st_info.f_mot_angle - f_angle3;																// 第1+2移動角度[rad]
//		printf("   [f_angle3]%d [f_angle1_2]%d\n\r", (int32_t)f_angle3, (int32_t)	st_info.f_angle1_2 );
	}
//	printf("[f_TrgtAngleS] %5.2f,st_info.f_angle1_2%5.2f,f_angle2%5.2f\n\r",f_TrgtAngleS,st_info.f_angle1_2,f_angle3);
*/
	st_data.en_ctrl_type			= CTRL_CONST_TRUN;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 目標速度
	st_data.f_ctrl_nowDist		= 0;						// 進んでいない
	st_data.f_ctrl_dist			= 0;						// 等速完了位置
//	st_data.f_ctrl_accAngleS		= 0;						// 角加速度
	st_data.f_ctrl_jerkAngle	= 0;
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= 0;				// 角加速度
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS;				// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_trgtAngleS;				// 目標角度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_angle1;			// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle1_2;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_setData( &st_data );							// データセット
	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		while( Get_NowAngle() > st_info.f_mot_angle1_2 ){			// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( Get_NowAngle() < st_info.f_mot_angle1_2 ){			// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
//	CTRL_clrAngleErrSum();

	/* ------ */
	/*  // 減速  */
	/* ------ */
	st_data.en_ctrl_type			= CTRL_DEC_TRUN;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 最終速度
	st_data.f_ctrl_nowDist		= 0;						// 等速完了位置
	st_data.f_ctrl_dist			= 0;						// 全移動完了位置
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle*(-1.0);
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle3*(-1.0);				// 角加速度
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS;				// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_trgtAngleS-st_info.f_mot_accAnglejerk_v;				// 目標角度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_angle1_2;			// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_setData( &st_data );							// データセット
	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		while( Get_NowAngle() > ( st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( Get_NowAngle() < ( st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk ) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}

	st_data.en_ctrl_type			= CTRL_DEC_TRUN;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 最終速度
	st_data.f_ctrl_nowDist		= 0;						// 等速完了位置
	st_data.f_ctrl_dist			= 0;						// 全移動完了位置
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS3;		// 角加速度
	st_data.f_ctrl_jerkAngle		= 0;
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle3*(-1.0);
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle3*(-1.0);
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS-st_info.f_mot_accAnglejerk_v;				// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_accAnglejerk_v;						// 目標角度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk;		// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle-st_info.f_mot_l3_accanglejerk;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_setData( &st_data );							// データセット
	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		while( Get_NowAngle() > ( st_info.f_mot_angle-st_info.f_mot_l3_accanglejerk) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( Get_NowAngle() < ( st_info.f_mot_angle-st_info.f_mot_l3_accanglejerk ) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}

	st_data.en_ctrl_type			= CTRL_DEC_TRUN;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 最終速度
	st_data.f_ctrl_nowDist		= 0;						// 等速完了位置
	st_data.f_ctrl_dist			= 0;						// 全移動完了位置
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS3;		// 角加速度
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle;
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle3*(-1.0);
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_accAnglejerk_v;				// 現在角速度
	st_data.f_ctrl_trgtAngleS		= 0;						// 目標角度
	st_data.f_ctrl_nowAngle		= st_info.f_mot_angle-st_info.f_mot_l3_accanglejerk;		// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_setData( &st_data );							// データセット
	if( ( en_type == MOT_R90 ) || ( en_type == MOT_R180 ) || ( en_type == MOT_R360 ) ){		// -方向
		while( Get_NowAngle() > ( st_info.f_mot_angle) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
			if((EscapeWait>0.5)&&(SearchFlag == TRUE))break;
		}
	}
	else{
		while( Get_NowAngle() < ( st_info.f_mot_angle ) ){		// 指定距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
			if((EscapeWait>0.5)&&(SearchFlag == TRUE))break;
		}
	}
//	printf("finish3\n");
	/* 停止 */
	LL_mDelay(200);				// 安定待ち
	CTRL_stop();			// 制御停止
	DCM_brakeMot( DCM_R );		// ブレーキ
	DCM_brakeMot( DCM_L );		// ブレーキ
	GYRO_endErrChkAngle();					// エラー検出終了
//	CTRL_clrNowData();
	CTRL_setNowData_Err(/*st_data.f_dist,*/st_data.f_ctrl_angle);
}

void MOT_setSlaStaSpeed( float f_speed , uint8_t sla_cmd)
{
	if(sla_cmd == SLA_90){
		f_MotSlaStaSpeed_90S = f_speed;
	}	else if(sla_cmd == SLA_45){
		f_MotSlaStaSpeed_45S = f_speed;
	}	else if(sla_cmd == SLA_135){
		f_MotSlaStaSpeed_135S = f_speed;
	}	else if(sla_cmd == SLA_N90){
		f_MotSlaStaSpeed_V90 = f_speed;
	}

}

float MOT_getSlaStaSpeed( uint8_t sla_cmd )
{
	float sla_speed;

	if(sla_cmd == SLA_90){
		sla_speed = f_MotSlaStaSpeed_90S;
	}else if(sla_cmd == SLA_45){
		sla_speed = f_MotSlaStaSpeed_45S;
	}else if(sla_cmd == SLA_135){
		sla_speed = f_MotSlaStaSpeed_135S;
	}else if(sla_cmd == SLA_N90){
		sla_speed = f_MotSlaStaSpeed_V90;
	}
	return sla_speed;
}

float MOT_setTrgtSpeed(float f_speed)
{
	f_MotTrgtSpeed = f_speed;
	return f_MotTrgtSpeed;
}

void MOT_setNowSpeed(float f_speed)
{
	f_MotNowSpeed = f_speed;
}

void MOT_goHitBackWall(void)
{
	stMOT_DATA	st_info;	//シーケンスデータ
	stCTRL_DATA	st_data;	//制御データ

	/* ---------------- */
	/*  動作データ計算  */
	/* ---------------- */
	/* 加速度 */
	st_info.f_mot_trgtAcc1= 1200.0;												// 角加速度1[rad/s^2]												// 角加速度3[rad/s^2]

	GYRO_staErrChkAngle();			// エラー検出開始
//	printf("");
	/* ================ */
	/*      実動作       */
	/* ================ */
	/* ------ */
	/*  加速  */
	/* ------ */
	st_data.en_ctrl_type			= CTRL_HIT_WALL;
	st_data.f_ctrl_jerk			= st_Info.f_mot_jerk;
	st_data.f_ctrl_trgtAcc		= st_info.f_mot_trgtAcc1;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 目標速度
	st_data.f_ctrl_nowDist		= 0;						// 進んでいない
	st_data.f_ctrl_dist			= 0;						// 加速距離
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= 0;		// 目標角度
	st_data.f_ctrl_nowAngle		= 0;						// 現在角度
	st_data.f_ctrl_angle			= 0;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_clrData();										// マウスの現在位置/角度をクリア
	CTRL_setData( &st_data );							// データセット
	DCM_staMotAll();									// モータON
//	printf("目標速度 %f 目標位置 %f\r\n",st_data.f_trgt,st_data.f_dist);

	/*停止*/
	LL_mDelay(400);				// 安定待ち
	CTRL_stop();			// 制御停止
	DCM_brakeMot( DCM_R );		// ブレーキ
	DCM_brakeMot( DCM_L );		// ブレーキ

	LL_mDelay(100);

	f_MotNowSpeed = 0.0f;		//現在速度更新

	GYRO_endErrChkAngle();					// エラー検出終了
	CTRL_clrNowData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
}

void MOT_goSla( enMOT_SLA_CMD en_type, stSLA* p_sla )
{
	stMOT_DATA		st_info;					// シーケンスデータ
	stCTRL_DATA		st_data;					// 制御データ
//	float			f_err;
	float			f_entryLen;
	float			f_escapeLen;

	float		f_accAngleTime1;
	float		f_accAngleTime3;
	float		f_mot_l1_accanglejerk;
	float		f_mot_l1_decanglejerk;
	float		f_mot_accAnglejerk_v;
    float		f_mot_l1_accangleconst;

	float		f_mot_l3_decanglejerk;
	float		f_mot_l3_accanglejerk;
    float		f_mot_l3_decangleconst;

	/* ---------------- */
	/*  動作データ計算  */
	/* ---------------- */

	st_Info.f_mot_jerk		= 0;
	/* 加速度 */
	st_info.f_mot_trgtAcc1 		= 0;																// 加速度1[mm/s^2]
	st_info.f_mot_trgtAcc3 		= 0;																// 加速度3[mm/s^2]

	/* 速度 */
	st_info.f_mot_now		= p_sla->f_sla_speed;													// 現在速度
	st_info.f_mot_trgt		= p_sla->f_sla_speed;													// 目標速度
	st_info.f_mot_last		= p_sla->f_sla_speed;													// 最終速度

	/* 距離 */
	st_info.f_mot_dist		= 0;																// 移動距離
	st_info.f_mot_l1		= 0;																// 第1移動距離[mm]
	st_info.f_mot_l1_2		= 0;																// 第1+2移動距離[mm]

	//角度jerk
	st_info.f_mot_jerkAngle = p_sla->f_sla_angJerk;
	/* 角加速度 */
	st_info.f_mot_trgtAccAngle1= p_sla->f_sla_angAcc;													// 角加速度1[deg/s^2]
	st_info.f_mot_trgtAccAngle3= p_sla->f_sla_angAcc;													// 角加速度3[deg/s^2]

	/* 角速度 */
	st_info.f_mot_nowAngleS	= 0;																// 現在角速度[deg/s]
	st_info.f_mot_trgtAngleS= p_sla->f_sla_angvel;													// 目標角速度
	st_info.f_mot_lastAngleS= 0;																// 最終角速度
	st_info.f_mot_accAnglejerk_v = p_sla->f_sla_angS_Jerk;

	/* 角度 */
	st_info.f_mot_angle				= p_sla->f_sla_ang_Total;
	st_info.f_mot_angle1			= p_sla->f_sla_ang_AccEnd;												// 第1移動角度[deg]
	st_info.f_mot_angle1_2			= p_sla->f_sla_ang_ConstEnd;											// 旋回角度[deg]
	
	st_info.f_mot_l1_accanglejerk	= p_sla->f_sla_ang_AccAccJerk;
	st_info.f_mot_l1_accangleconst	= p_sla->f_sla_ang_AccConst;
	st_info.f_mot_l1_decanglejerk	= p_sla->f_sla_ang_AccDecJerk;											// 第1+2移動角度[deg]
	st_info.f_mot_l3_decanglejerk	= p_sla->f_sla_ang_DecDecJerk;
	st_info.f_mot_l3_decangleconst	= p_sla->f_sla_ang_DecConst;
	st_info.f_mot_l3_accanglejerk	= p_sla->f_sla_ang_DecAccJerk;

	/* 方向に応じて符号を変更 */
	if( ( en_type == MOT_R90S ) ||
		( en_type == MOT_R45S_S2N ) || ( en_type == MOT_R45S_N2S ) ||
		( en_type == MOT_R90S_N ) ||
		( en_type == MOT_R135S_S2N ) || ( en_type == MOT_R135S_N2S )
	){
		st_info.f_mot_jerkAngle *= -1.0;
		st_info.f_mot_trgtAccAngle1 *= -1.0;
		st_info.f_mot_trgtAngleS *= -1.0;
		st_info.f_mot_accAnglejerk_v *= -1.0;

		st_info.f_mot_angle      *= -1.0;
		st_info.f_mot_angle1     *= -1.0;
		st_info.f_mot_angle1_2   *= -1.0;

		st_info.f_mot_l1_accanglejerk	*= -1.0;
		st_info.f_mot_l1_accangleconst	*= -1.0;
		st_info.f_mot_l1_decanglejerk	*= -1.0;
		st_info.f_mot_l3_decanglejerk	*= -1.0;
		st_info.f_mot_l3_decangleconst	*= -1.0;
		st_info.f_mot_l3_accanglejerk	*= -1.0;
	}
	else{
		st_info.f_mot_trgtAccAngle3 *= -1.0;
	}

	/* 斜め走行のタイプに応じて、スラローム前の距離とスラローム後の退避距離を入れ替える */
	if( ( en_type == MOT_R45S_N2S ) || ( en_type == MOT_L45S_N2S ) || ( en_type == MOT_R135S_N2S ) || ( en_type == MOT_L135S_N2S ) ){ 		// 逆にするもの
		f_entryLen  = p_sla->f_sla_escapeLen;
		f_escapeLen = p_sla->f_sla_entryLen;
	}
	else{		// 通常
		f_entryLen  = p_sla->f_sla_entryLen;
		f_escapeLen = p_sla->f_sla_escapeLen;
	}

	GYRO_staErrChkAngle();			// エラー検出開始

//	LED_on(LED1);
	/* ================ */
	/*      entry      */
	/* ================ */
	/* ------------------------ */
	/*  スラローム前の前進動作  */
	/* ------------------------ */
	st_data.en_ctrl_type			= CTRL_ENTRY_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= 0;						// 進んでいない
	st_data.f_ctrl_dist			= f_entryLen;				// スラローム前の前進距離
	st_data.f_ctrl_jerkAngle	= 0;
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
	st_data.f_ctrl_trgtAngleS	= 0;						// 目標角度
	st_data.f_ctrl_nowAngle		= 0;						// 現在角度
	st_data.f_ctrl_angle			= 0;						// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_clrData();										// マウスの現在位置/角度をクリア
	CTRL_setData( &st_data );							// データセット
	DCM_staMotAll();									// モータON

	while( Get_NowDist() < f_entryLen + uc_DistControl ){				// 指定距離到達待ち
		if(( en_type == MOT_R90S ) ||( en_type == MOT_L90S ) ){
			if((DIST_getNowVal( DIST_SEN_R_FRONT )>R_FRONT_CTRL)&&(DIST_getNowVal( DIST_SEN_L_FRONT )>L_FRONT_CTRL))break;
		}
		else{
			uc_DistControl = 0.0;
		}
		if( SYS_isOutOfCtrl() == TRUE ){
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			break;
		}				// 途中で制御不能になった
	}
//	LED_off(LED1);
//	log_in(0);
	/* ------ */
	/*  acc jerk */
	/* ------ */
	st_data.en_ctrl_type			= CTRL_ACC_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen;				//
	st_data.f_ctrl_dist			= f_entryLen + st_info.f_mot_now * p_sla->us_sla_jerkAngaccTime * 0.001;		// 加速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS1;		// 角加速度
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle;
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle1;
	st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_accAnglejerk_v;		// 目標角速度
	st_data.f_ctrl_nowAngle		= 0;						// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_l1_accanglejerk;			// 目標角度
	st_data.f_ctrl_time 			= p_sla->us_sla_jerkAngaccTime * 0.001;			// [msec] → [sec]
	CTRL_setData( &st_data );							// データセット
//	printf("trgtangleS %5.2f\n\r",st_data.f_trgtAngleS);
//	printf("jerkacc angle%.5f, dist%.5f\n\r",st_info.f_mot_l1_accanglejerk,st_data.f_ctrl_dist);
	if( IS_R_SLA( en_type ) == TRUE ) {		// -方向
		while( ( Get_NowAngle() > st_info.f_mot_l1_accanglejerk ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( ( Get_NowAngle() < st_info.f_mot_l1_accanglejerk ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	//acc const
	st_data.en_ctrl_type			= CTRL_ACC_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen + st_info.f_mot_now * p_sla->us_sla_jerkAngaccTime * 0.001;				//
	st_data.f_ctrl_dist			= f_entryLen + st_info.f_mot_now * (p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime)* 0.001;		// 加速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS1;		// 角加速度
	st_data.f_ctrl_jerkAngle	= 0;
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle1;
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle1;
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_accAnglejerk_v;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_trgtAngleS-st_info.f_mot_accAnglejerk_v;		// 目標角速度
	st_data.f_ctrl_nowAngle		= st_info.f_mot_l1_accanglejerk;						// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst;			// 目標角度
	st_data.f_ctrl_time 			=  p_sla->us_sla_accAngvelTime * 0.001;			// [msec] → [sec]
	CTRL_setData( &st_data );							// データセット
//	printf("trgtangleS %5.2f\n\r",st_data.f_trgtAngleS);
//	printf("jerkconst angle%.5f, dist%.5f\n\r",st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst,st_data.f_ctrl_dist);
	if( IS_R_SLA( en_type ) == TRUE ) {		// -方向
		while( ( Get_NowAngle() > (st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst) ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( ( Get_NowAngle() < (st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst) ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}

	//acc -jerk
	st_data.en_ctrl_type			= CTRL_ACC_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen + st_info.f_mot_now * (p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime)* 0.001;				//
	st_data.f_ctrl_dist			= f_entryLen + st_info.f_mot_now * (p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime*2.0)* 0.001;		// 加速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS1;		// 角加速度
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle*(-1.0);
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle1;
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS - st_info.f_mot_accAnglejerk_v;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_trgtAngleS;		// 目標角速度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_l1_accanglejerk+st_info.f_mot_l1_accangleconst;						// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle1;			// 目標角度
	st_data.f_ctrl_time 			=  p_sla->us_sla_jerkAngaccTime * 0.001;			// [msec] → [sec]
	CTRL_setData( &st_data );							// データセット
//	printf("trgtangleS %5.2f\n\r",st_data.f_trgtAngleS);
//	printf("jerkdec angle%.5f, dist%.5f\n\r",st_info.f_mot_angle1,st_data.f_ctrl_dist);
	if( IS_R_SLA( en_type ) == TRUE ) {		// -方向
		while( ( Get_NowAngle() > st_info.f_mot_angle1 ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( ( Get_NowAngle() < st_info.f_mot_angle1 ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}

//	log_in(0);
//	log_in(f_NowAngle);
	/* ------ */
	/*  const  */
	/* ------ */
	st_data.en_ctrl_type			= CTRL_CONST_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen + st_info.f_mot_now * (p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime*2.0)* 0.001;
	st_data.f_ctrl_dist			= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime*2.0) * 0.001;		// 等速距離
//	st_data.f_ctrl_accAngleS		= 0;						// 角加速度
	st_data.f_ctrl_jerkAngle	= 0;
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS;		// 現在角速度
	st_data.f_ctrl_trgtAngleS	= st_info.f_mot_trgtAngleS;		// 目標角速度
	st_data.f_ctrl_nowAngle		= st_info.f_mot_angle1;			// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle1_2;		// 目標角度
	st_data.f_ctrl_time 			= p_sla->us_sla_constAngvelTime * 0.001;		// [msec] → [sec]
	CTRL_setData( &st_data );							// データセット
//	printf("const angle%.5f, dist%.5f\n\r",st_info.f_mot_angle1_2,st_data.f_ctrl_dist);
	if( IS_R_SLA( en_type ) == TRUE ) {		// -方向
		while( ( Get_NowAngle() > st_info.f_mot_angle1_2 ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){		// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( ( Get_NowAngle() < st_info.f_mot_angle1_2 ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){		// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
//	log_in(0);
//	log_in(f_NowAngle);
		
	/* ------ */
	/*  dec  */
	/* ------ */
	//dec -jerk
	st_data.en_ctrl_type			= CTRL_DEC_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime*2.0)* 0.001;		//
	st_data.f_ctrl_dist			= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime*3.0)* 0.001;		// 加速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS1;		// 角加速度
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle*(-1.0);
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle3;
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_trgtAngleS - st_info.f_mot_accAnglejerk_v;		// 目標角速度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_angle1_2;						// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk;			// 目標角度
	st_data.f_ctrl_time 			=  p_sla->us_sla_jerkAngaccTime * 0.001;			// [msec] → [sec]
	CTRL_setData( &st_data );							// データセット
//	printf("trgtangleS %5.2f\n\r",st_data.f_trgtAngleS);
//	printf("jerkacc angle%.5f, dist%.5f\n\r",st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk,st_data.f_ctrl_dist);
	if( IS_R_SLA( en_type ) == TRUE ) {		// -方向
		while( ( Get_NowAngle() > (st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk) ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}
	else{
		while( ( Get_NowAngle() < (st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk) ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった
		}
	}

	st_data.en_ctrl_type			= CTRL_DEC_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime + p_sla->us_sla_jerkAngaccTime*3.0)* 0.001;
	st_data.f_ctrl_dist			= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime*2.0 + p_sla->us_sla_jerkAngaccTime*3.0)* 0.001;		// 減速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS3;		// 角加速度
	st_data.f_ctrl_jerkAngle	= 0;
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle3;
	st_data.f_ctrl_trgtAccAngle		= st_info.f_mot_trgtAccAngle3;
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_trgtAngleS - st_info.f_mot_accAnglejerk_v;		// 現在角速度
	st_data.f_ctrl_trgtAngleS		= st_info.f_mot_accAnglejerk_v;				// 目標角速度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_angle1_2+st_info.f_mot_l3_decanglejerk;		// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle - st_info.f_mot_l3_accanglejerk;			// 目標角度
	st_data.f_ctrl_time			= p_sla->us_sla_accAngvelTime * 0.001;			// [msec] → [sec]
	CTRL_setData( &st_data );							// データセット
//	LED = LED_ALL_ON;
//	printf("jerkconst angle%.5f, dist%.5f\n\r",st_info.f_mot_angle - st_info.f_mot_l3_accanglejerk,st_data.f_ctrl_dist);
	if( IS_R_SLA( en_type ) == TRUE ) {		// -方向
		while( ( Get_NowAngle() > (st_info.f_mot_angle - st_info.f_mot_l3_accanglejerk) ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった

		}
	}
	else{
		while( ( Get_NowAngle() < (st_info.f_mot_angle - st_info.f_mot_l3_accanglejerk) ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった

		}
	}

	//dec jerk
	st_data.en_ctrl_type			= CTRL_DEC_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime*2.0 + p_sla->us_sla_jerkAngaccTime*3.0)* 0.001;
	st_data.f_ctrl_dist			= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime*2.0 + p_sla->us_sla_jerkAngaccTime*4.0)* 0.001;		// 減速距離
//	st_data.f_ctrl_accAngleS		= st_info.f_mot_accAngleS3;		// 角加速度
	st_data.f_ctrl_jerkAngle	= st_info.f_mot_jerkAngle;
	st_data.f_ctrl_nowAccAngle		= st_info.f_mot_trgtAccAngle3;
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= st_info.f_mot_accAnglejerk_v;		// 現在角速度
	st_data.f_ctrl_trgtAngleS		= 0;				// 目標角速度
	st_data.f_ctrl_nowAngle			= st_info.f_mot_angle - st_info.f_mot_l3_accanglejerk;		// 現在角度
	st_data.f_ctrl_angle			= st_info.f_mot_angle;			// 目標角度
	st_data.f_ctrl_time			= p_sla->us_sla_jerkAngaccTime * 0.001;			// [msec] → [sec]
	CTRL_setData( &st_data );							// データセット
//	LED = LED_ALL_ON;
//	printf("jerkdec angle%.5f, dist%.5f\n\r",st_info.f_mot_angle,st_data.f_ctrl_dist);
	if( IS_R_SLA( en_type ) == TRUE ) {		// -方向
		while( ( Get_NowAngle() > st_info.f_mot_angle ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった

		}
	}
	else{
		while( ( Get_NowAngle() < st_info.f_mot_angle ) && ( Get_NowDist() < st_data.f_ctrl_dist ) ){			// 指定角度＋距離到達待ち
			if( SYS_isOutOfCtrl() == TRUE ){
				CTRL_stop();
				DCM_brakeMot( DCM_R );		// ブレーキ
				DCM_brakeMot( DCM_L );		// ブレーキ
				break;
			}				// 途中で制御不能になった

		}
	}


//	LED_on(LED1);
	/* ------------------------ */
	/*  escape  */
	/* ------------------------ */
	st_data.en_ctrl_type			= CTRL_EXIT_SLA;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= st_info.f_mot_now;			// 現在速度
	st_data.f_ctrl_trgt			= st_info.f_mot_now;			// 目標速度
	st_data.f_ctrl_nowDist		= f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime*2.0 + p_sla->us_sla_jerkAngaccTime*4.0) * 0.001;
	st_data.f_ctrl_dist			= f_escapeLen + f_entryLen + st_info.f_mot_now * ( p_sla->us_sla_constAngvelTime + p_sla->us_sla_accAngvelTime*2.0 + p_sla->us_sla_jerkAngaccTime*4.0) * 0.001;	// スラローム後の前進距離
//	st_data.f_ctrl_accAngleS		= 0;						// 角加速度
	st_data.f_ctrl_jerkAngle		= 0;
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= 0;						// 目標角度
	st_data.f_ctrl_nowAngle		= 0;						// 現在角度
	st_data.f_ctrl_angle			= 0;						// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_setData( &st_data );							// データセット
//	LED =LED_ALL_OFF;
	while( Get_NowDist() < ( st_data.f_ctrl_dist ) ){	// 指定距離到達待ち
		if( SYS_isOutOfCtrl() == TRUE ){
			CTRL_stop();
			DCM_brakeMot( DCM_R );		// ブレーキ
			DCM_brakeMot( DCM_L );		// ブレーキ
			break;
		}				// 途中で制御不能になった
	}
//	LED_off(LED1);
//	log_in(f_NowAngle);
	f_MotNowSpeed = st_info.f_mot_now;			// 現在速度更新
//	LED =LED_ALL_OFF;
	GYRO_endErrChkAngle();					// エラー検出終了
//	CTRL_clrNowData();
	CTRL_setNowData_Err(/*st_data.f_dist ,*/st_data.f_ctrl_angle);

}

void turntable(void)
{
	stCTRL_DATA test;
		test.en_ctrl_type = CTRL_CONST;
		test.f_ctrl_jerk			= 0;
		test.f_ctrl_trgtAcc		= 0;						// 加速度指定
		test.f_ctrl_nowAcc		= 0;
		test.f_ctrl_now			= 0;			// 現在速度
		test.f_ctrl_trgt			= 0;			// 目標速度
		test.f_ctrl_nowDist			= 0;
		test.f_ctrl_dist			= 0;
//		test.f_ctrl_accAngleS		= 0;						// 角加速度
		test.f_ctrl_nowAccAngle		= 0;
		test.f_ctrl_trgtAccAngle		= 0;
		test.f_ctrl_nowAngleS		= GYRO_getSpeedErr();						// 現在角速度
		test.f_ctrl_trgtAngleS		= 0;						// 目標角度
		test.f_ctrl_nowAngle			= GYRO_getNowAngle();						// 現在角度
		test.f_ctrl_angle			= 0;						// 目標角度
		test.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない

	CTRL_clrData();
	CTRL_clrNowData();
	CTRL_setData(&test);
	DCM_staMotAll();									// モータON
	while( 1 ){	// 指定距離到達待ち
		if ( SW_ON == SW_IsOn_0() ){
			CTRL_stop();
			break;
		}
	}
}

void MOT_setWallEdgeType( enMOT_WALL_EDGE_TYPE en_type )
{
	en_WallEdge = en_type;
	bl_IsWallEdge = FALSE;			// 非検知

}

enMOT_WALL_EDGE_TYPE MOT_getWallEdgeType( void )
{
	return en_WallEdge;
}

void MOT_setWallEdge( bool bl_val )
{
	bl_IsWallEdge = bl_val;

}

bool MOT_setWallEdgeDist( void )
{
	float f_addDist;

	/* 壁の切れ目を検知していない */
	if( ( bl_IsWallEdge == FALSE ) || ( en_WallEdge == MOT_WALL_EDGE_NONE ) ){		// 壁切れ設定されていないか、検出していない場合は処理を抜ける
		return FALSE;
	}

	f_addDist = Get_NowDist() + MOT_WALL_EDGE_DIST;		// 旋回開始位置

	/* 多く走る必要がある */
	if( f_addDist > st_Info.f_mot_dist ){

		f_WallEdgeAddDist = f_addDist - st_Info.f_mot_dist;	//壁切れの距離＋現在距離が本来の距離を超えた分をwall_adddistとして設定してるんだなぁだから設定だけすればいいのか
	}
	/*少なく走る場合*/
/*	if( f_addDist < st_Info.f_dist){
		st_Info.f_dist = f_addDist;
	}
*/
	/* 壁の切れ目補正の変数を初期化 */
	en_WallEdge   = MOT_WALL_EDGE_NONE;		// 壁の切れ目タイプ
	bl_IsWallEdge = FALSE;					// 壁の切れ目検知
	return TRUE;
}
bool MOT_setWallEdgeDist_LoopWait( void )
{
//	SetLED(0x11);
	/* 壁の切れ目を検知していない */
	if( bl_IsWallEdge == FALSE ){		// 壁切れ設定されていないか、検出していない場合は処理を抜ける

		return FALSE;
	}

	f_WallEdgeAddDist = MOT_WALL_EDGE_DIST;		// 旋回開始位置

	return TRUE;
}

void DIST_Front_Wall_correction(void)
{
	stMOT_DATA	st_info;	//シーケンスデータ
	stCTRL_DATA	st_data;	//制御データ

	GYRO_staErrChkAngle();			// エラー検出開始
	
	st_data.en_ctrl_type			= CTRL_FRONT_WALL;
	st_data.f_ctrl_jerk			= 0;
	st_data.f_ctrl_trgtAcc		= 0;						// 加速度指定
	st_data.f_ctrl_nowAcc		= 0;
	st_data.f_ctrl_now			= 0;						// 現在速度
	st_data.f_ctrl_trgt			= 0;						// 目標速度
	st_data.f_ctrl_nowDist		= 0;						// 進んでいない
	st_data.f_ctrl_dist			= 0;						// 加速距離
//	st_data.f_ctrl_accAngleS		= 0;		// 角加速度
	st_data.f_ctrl_nowAccAngle		= 0;
	st_data.f_ctrl_trgtAccAngle		= 0;
	st_data.f_ctrl_nowAngleS		= 0;						// 現在角速度
	st_data.f_ctrl_trgtAngleS		= 0;		// 目標角度
	st_data.f_ctrl_nowAngle		= 0;						// 現在角度
	st_data.f_ctrl_angle			= 0;			// 目標角度
	st_data.f_ctrl_time 			= 0;						// 目標時間 [sec] ← 指定しない
	CTRL_clrData();										// マウスの現在位置/角度をクリア
	CTRL_setData( &st_data );							// データセット
	DCM_staMotAll();									// モータON
	while((DIST_getNowVal( DIST_SEN_R_FRONT )>(R_FRONT_REF+FRONT_WALL_MINUS+30))||(DIST_getNowVal( DIST_SEN_R_FRONT )<(R_FRONT_REF+FRONT_WALL_MINUS-30))
		||(DIST_getNowVal( DIST_SEN_L_FRONT )>(L_FRONT_REF+FRONT_WALL_MINUS+30))||(DIST_getNowVal( DIST_SEN_L_FRONT )<(L_FRONT_REF+FRONT_WALL_MINUS-30))){
			if(EscapeWait > 0.8)break;
			LL_mDelay(10);//volatile入れてないから回避用に入れてみる
	}
	LL_mDelay(50);
	CTRL_stop();			// 制御停止
	DCM_brakeMot( DCM_R );		// ブレーキ
	DCM_brakeMot( DCM_L );		// ブレーキ
	GYRO_endErrChkAngle();					// エラー検出終了
	LL_mDelay(100);
	f_MotNowSpeed = 0.0f;		//現在速度更新

	CTRL_clrNowData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
}

