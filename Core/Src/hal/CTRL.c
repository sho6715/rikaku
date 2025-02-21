/*
 * CTRL.c
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#include "hal/CTRL.h"


/* control  */
uint8_t 			uc_CtrlFlag			= FALSE;	// FF,FB active Flag (1:active)
int32_t			l_CntR;							// right motor count (updated 1[msec])
int32_t			l_CntL;							// left motor count (updated 1[msec])
float			f_Time 				= 0;		// operating time[sec] (updated 1[msec])
float			f_TrgtTime 			= 1000;		// operating target time [msec]	(set value)

float			f_Jerk			= 0;
float			f_BaseAcc		= 0;	//start accel
float			f_TrgtAcc			= 0;	//now accel
float			f_LastAcc		= 0;
// velocity control
//float 			f_Acc			= 0;		// [velocity control]   acceleration[m/s2]	(set value)
float			f_BaseSpeed		= 0;		// [velocity control]   initial velocity[m/s]	(set value)
float			f_LastSpeed 		= 0;		// [velocity control]   last target velocity[m/s]	(set value)
float			f_NowSpeed		= 0;		// [velocity control]   now velocity [m/s]	(updated 1[msec])
float			f_TrgtSpeed 		= 0;		// [velocity control]   target velocity[m/s]		(updated 1[msec])
float			f_ErrSpeedBuf		= 0;		// [velocity control] 縲velocity error buffer	(updated 1[msec])
float			f_SpeedErrSum 		= 0;		// [velocity control]   sum of velocity integral control	(updated 1[msec])

float			f_MotorR_AngleS = 0;
float			f_MotorL_AngleS = 0;

// dist
float			f_BaseDist		= 0;		// [dist]   initial distance[m]		(set value)
float			f_LastDist 		= 0;		// [距離制御]   最終移動距離					（設定値）
float			f_TrgtDist 		= 0;		// [距離制御]   目標移動距離					（1[msec]毎に更新される）
volatile float 		f_NowDist		= 0;		// [距離制御]   現在距離						（1[msec]毎に更新される）
float			f_NowDistR		= 0;		// [距離制御]   現在距離（右）					（1[msec]毎に更新される）
float 			f_NowDistL		= 0;		// [距離制御]   現在距離（左）					（1[msec]毎に更新される）
float			f_DistErrSum 		= 0;		// [距離制御]   距離積分制御のサム値			（1[msec]毎に更新される）

float			f_JerkAngle		=0;
float			f_BaseAccAngle		= 0;	//start accel
float			f_TrgtAccAngle			= 0;	//now accel
float			f_LastAccAngle		= 0;

// angular velocity control
//float 			f_AccAngleS		= 0;		// [angle velocity control] angular acceleration[rad/s2]	(set value)
float			f_BaseAngleS		= 0;		// [angle velocity control] initial angular velocity[rad/s]		(set value)
float			f_LastAngleS 		= 0;		// [angle velocity control] last target angular velocity	(set value)
float			f_TrgtAngleS 		= 0;		// [angle velocity control] target angular velocity [rad/s]	(updated 1[msec])
float			f_ErrAngleSBuf		= 0;		// [angle velocity control] angular velocity buffer	(updated 1[msec])
float			f_AngleSErrSum 		= 0;		// [angle velocity control] sum of angular velocity integral contorol	(updated 1[msec])
// angle
float			f_BaseAngle		= 0;		// [角度制御]   初期角度						（設定値）
float			f_LastAngle 		= 0;		// [角度制御]   最終目標角度					（設定値）
volatile float 		f_NowAngle		= 0;		// [角度制御]   現在角度　	volatileをつけないとwhileから抜けられなくなる（最適化のせい）（1[msec]毎に更新される）
float			f_TrgtAngle 		= 0;		// [角度制御]   目標角度						（1[msec]毎に更新される）
float			f_AngleErrSum 		= 0;		// [角度制御]   角度積分制御のサム値			（1[msec]毎に更新される）
// wall control
int32_t 			l_WallErr 		= 0;		// [wall control]     wall error		(updated 1[msec])
float			f_ErrDistBuf		= 0;		// [wall control]     wall error buffer	(updated 1[msec])	

int32_t			l_FrontSen_vErr		=0;
int32_t			l_FrontSen_OmegaErr		=0;
float			f_ErrFrontSen_vBuf	= 0;
float			f_ErrFrontSen_OmegaBuf	= 0;

//fail safe
float  			f_ErrChkAngle; 			  // ジャイロセンサのエラー検出用の角度
bool   			bl_ErrChk; 				  // ジャイロセンサのエラー検出（FALSE：検知しない、TRUE：検知する）
bool			bl_Failsafe		= FALSE;	// マウスがの制御不能（TRUE：制御不能、FALSE：制御可能）

enCTRL_TYPE		en_Type;						// motion type
//escape time 
float	EscapeWait;

uint8_t	uc_DistControl;	//スラロームの前距離を伸ばして処理している箇所　意外と効いてる可能性あり


float Get_NowSpeed(void){
	return f_NowSpeed;
}

float Get_NowDist(void){
	return f_NowDist;
}

float Get_TrgtDist(void){
	return f_TrgtDist;
}

float Get_TrgtSpeed(void){
	return f_TrgtSpeed;
}

void Set_TrgtSpeed(float speed){
	f_TrgtSpeed = speed;
}

float Get_NowDistR(void){
	return f_NowDistR;
}

float Get_NowAngle(void){
	return f_NowAngle;
}

float Get_TrgtAngle(void){
	return f_TrgtAngle;
}

float Get_TrgtAngleS(void){
	return f_TrgtAngleS;
}

void INTC_sys(void)
{
	Msec_in++;					// msec
	if( Msec_in > 999 ){		// msec → sec
		Msec_in  = 0;
		Sec_in++;
	}
	if( Sec_in > 59 ){			// sec → min
		Sec_in = 0;
		Min_in++;
	}

	CTRL_pol();
}

void CTRL_sta( void )
{
	uc_CtrlFlag = TRUE;
}

void CTRL_stop( void )
{
	uc_CtrlFlag = FALSE;
	DCM_brakeMot( DCM_R );		// ブレーキ
	DCM_brakeMot( DCM_L );		// ブレーキ
}

void CTRL_clrData( void )
{
/*	ENC_clr();
	l_CntR			= 0;						// カウンタクリア
	l_CntL			= 0;						// カウンタクリア
*/
	/* 現在値 */
/*	f_NowDist 		= 0;						// 移動距離リセット
	f_NowDistR 		= 0;
	f_NowDistL 		= 0;
	f_NowSpeed		= 0;						// [速度制御]   現在の速度 [mm/s]			（1[msec]毎に更新される）
	f_NowAngle		= 0;						// [角度制御]   現在角度					（1[msec]毎に更新される）
	s_GyroVal		= 0;						// ジャイロ値クリア
	f_GyroNowAngle	= 0;							// ジャイロ値クリア
*/
	/* 目標値 */
	f_TrgtSpeed		= 0;						// [速度制御]   目標移動速度 [mm/s]			（1[msec]毎に更新される）
	f_TrgtDist 		= 0;						// [距離制御]   目標移動距離				（1[msec]毎に更新される）
	f_TrgtAngleS	= 0;							// [角速度制御] 目標角速度 [rad/s]			（1[msec]毎に更新される）
	f_TrgtAngle		= 0;						// [角度制御]   目標角度					（1[msec]毎に更新される）

	/* 制御データ */
//	f_SpeedErrSum	= 0;
//	f_DistErrSum 	= 0;						// [距離制御]   距離積分制御のサム値			（1[msec]毎に更新される）
//	f_AngleSErrSum	= 0;
//	f_AngleErrSum 	= 0;						// [角度制御]   角度積分制御のサム値			（1[msec]毎に更新される）
	f_ErrSpeedBuf	= 0;
	f_ErrDistBuf	= 0;						// [壁制御]     距離センサーエラー値のバッファ		（1[msec]毎に更新される）
	f_ErrAngleSBuf  = 0;
	l_FrontSen_vErr		=0;
	l_FrontSen_OmegaErr		=0;
	f_ErrFrontSen_vBuf	= 0;
	f_ErrFrontSen_OmegaBuf	= 0;
}

void CTRL_clrAngleErrSum(void){
	f_AngleErrSum = 0.0;
	f_AngleSErrSum = 0.0;
}

void CTRL_clrSpeedErrSum(void){
	f_SpeedErrSum	= 0;
	f_DistErrSum 	= 0;
}

void CTRL_clrNowData(void)
{
	ENC_clr();
	l_CntR			= 0;						// カウンタクリア
	l_CntL			= 0;						// カウンタクリア

	/* 現在値 */
	f_NowDist 		= 0;						// 移動距離リセット
	f_NowDistR 		= 0;
	f_NowDistL 		= 0;
	f_NowSpeed		= 0;						// [速度制御]   現在の速度 [mm/s]			（1[msec]毎に更新される）
	f_NowAngle		= 0;						// [角度制御]   現在角度					（1[msec]毎に更新される）
	s_GyroVal		= 0;						// ジャイロ値クリア
	f_GyroNowAngle	= 0;							// ジャイロ値クリア
}

void CTRL_setNowData_Err(/*float trgtDist, */float trgtAngle)
{
	ENC_clr();
	l_CntR			= 0;						// カウンタクリア
	l_CntL			= 0;						// カウンタクリア

	/* 現在値 */
	f_NowDist 		= 0;//f_NowDist - trgtDist;//本来こっちにしたいが妥協						// 移動距離リセット
	f_NowDistR 		= 0;
	f_NowDistL 		= 0;
	f_NowSpeed		= 0;						// [速度制御]   現在の速度 [mm/s]			（1[msec]毎に更新される）
	f_NowAngle		= f_NowAngle - trgtAngle;						// [角度制御]   現在角度					（1[msec]毎に更新される）
	s_GyroVal		= 0;						// ジャイロ値クリア
	f_GyroNowAngle	= 0;							// ジャイロ値クリア
}

void CTRL_setData( stCTRL_DATA* p_data )
{
	/* 制御方法 */
	en_Type					= p_data->en_ctrl_type;

	f_Jerk					= p_data->f_ctrl_jerk;

	f_TrgtAcc				= p_data->f_ctrl_nowAcc;
	f_BaseAcc				= p_data->f_ctrl_nowAcc;
	f_LastAcc				= p_data->f_ctrl_trgtAcc;

	/* 速度制御 */
//	f_TrgtAcc 				= p_data->f_ctrl_acc;
	f_TrgtSpeed				= p_data->f_ctrl_now;
	f_BaseSpeed				= p_data->f_ctrl_now;
	f_LastSpeed				= p_data->f_ctrl_trgt;

	/* 距離制御 */
	f_TrgtDist				= p_data->f_ctrl_nowDist;
	f_BaseDist 				= p_data->f_ctrl_nowDist;
	f_LastDist 				= p_data->f_ctrl_dist;

	//角加速度
	f_JerkAngle				= p_data->f_ctrl_jerkAngle;
	f_TrgtAccAngle			= p_data->f_ctrl_nowAccAngle;
	f_BaseAccAngle			= p_data->f_ctrl_nowAccAngle;
	f_LastAccAngle			= p_data->f_ctrl_trgtAccAngle;

	/* 角速度制御 */
//	f_AccAngleS 			= p_data->f_ctrl_accAngleS;
	f_TrgtAngleS			= p_data->f_ctrl_nowAngleS;
	f_BaseAngleS			= p_data->f_ctrl_nowAngleS;
	f_LastAngleS			= p_data->f_ctrl_trgtAngleS;

	/* 角度制御 */
	f_TrgtAngle				= p_data->f_ctrl_nowAngle;
	f_BaseAngle 			= p_data->f_ctrl_nowAngle;
	f_LastAngle 			= p_data->f_ctrl_angle;

	f_Time 					= 0;
	f_TrgtTime				= p_data->f_ctrl_time;

	EscapeWait			= 0;

	CTRL_sta();				// 制御開始

}

void CTRL_refNow( void )
{
	float f_speedR		= 0;							// 右モータ現在速度 [mm/s]
	float f_speedL		= 0;							// 左モータ現在速度 [mm/s]
	float f_r 			= F_CNT2MM(l_CntR);				// 右モータの進んだ距離 [mm]
	float f_l 			= F_CNT2MM(l_CntL);				// 左モータの進んだ距離 [mm]

	/* 速度更新 */
	f_speedR = f_r * 1000.0;								// 右モータ速度 [mm/s] ( 移動距離[カウント] * 1パルスの移動量(0.0509[mm]) * 1000(msec→sec) 
	f_speedL = f_l * 1000.0;								// 左モータ速度 [mm/s] ( 移動距離[カウント] * 1パルスの移動量(0.0509[mm]) * 1000(msec→sec) 
	f_NowSpeed  = ( f_speedR + f_speedL ) / 2;			// マウス（進行方向中心軸） [1mm/s] 
	
	/*motor AngleS*/
	f_MotorR_AngleS	= f_speedR *GEAR_RATIO*2.0/TIRE_D;
	f_MotorL_AngleS = f_speedL *GEAR_RATIO*2.0/TIRE_D;

	/* 距離更新 */
	f_NowDistR += f_r;									// カウント更新
	f_NowDistL += f_l;									// カウント更新
	f_NowDist  = ( f_NowDistR + f_NowDistL ) / 2.0;		// 平均値更新
//	f_NowDist  += (f_r + f_l) / 2.0; 	//setNowdataの残り　うまく行かないため上に戻し
}

void CTRL_refTarget( void )
{
	/* mode */
	switch( en_Type ){

		/* acc(straight) */
		case CTRL_ACC:
		case CTRL_SKEW_ACC:
			f_TrgtAcc += f_Jerk*0.001;

			if(f_BaseAcc > 0){
				if(f_TrgtAcc < 0.0){
					f_TrgtAcc = 0.0;
				}
			}else{
				if(f_TrgtAcc > f_LastAcc){
					f_TrgtAcc = f_LastAcc;
				}
			}

			if( f_TrgtSpeed < (f_LastSpeed -(f_TrgtAcc * 0.001)) ){								// 加速目標更新区間
//			if( f_TrgtSpeed < f_LastSpeed ){
				f_TrgtSpeed += f_TrgtAcc * 0.001;									// 目標速度
			}
			else{
				f_TrgtSpeed = f_LastSpeed;
			}

			break;

		/* const(straight) */
		case CTRL_CONST:
		case CTRL_SKEW_CONST:
			f_TrgtSpeed = f_BaseSpeed;														// 目標速度
			break;

		/* dec(straight) */
		case CTRL_DEC:
		case CTRL_SKEW_DEC:
			f_TrgtAcc += f_Jerk*0.001;

			if(f_BaseAcc < 0){
				if(f_TrgtAcc > 0.0){
					f_TrgtAcc = 0.0;
				}
			}else{
				if(f_TrgtAcc < f_LastAcc){
					f_TrgtAcc = f_LastAcc;
				}
			}

			/* speed CTRL + position CTRL */
			if( f_TrgtSpeed > (f_LastSpeed +(f_TrgtAcc * 0.001))){								// 減速目標更新区間
//			if( f_TrgtSpeed > f_LastSpeed){								// 減速目標更新区間
				f_TrgtSpeed += f_TrgtAcc * 0.001;									// 目標速度
				f_TrgtDist  += f_TrgtSpeed * 0.001;		// 目標距離
			}
			/* position CTRL */
			else{
				f_TrgtSpeed = f_LastSpeed;
				f_TrgtDist  = f_LastDist;													// 目標距離
			}
			break;

		/* acc(Turn) */
		case CTRL_ACC_TRUN:
			f_TrgtAccAngle += f_JerkAngle*0.001;

			/* CCW  hidari*/
			if( f_LastAngle > 0 ){
				if(f_BaseAccAngle > 0){
					if(f_TrgtAccAngle < 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle > f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				if ( f_TrgtAngleS < (f_LastAngleS -(f_TrgtAccAngle * 0.001)) ){
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;									// 目標角速度
				}
				else{
					f_TrgtAngleS = f_LastAngleS;
				}
			}
			/* CW  migi */
			else{
				if(f_BaseAccAngle < 0){
					if(f_TrgtAccAngle > 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle < f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				if( f_TrgtAngleS > (f_LastAngleS +(f_TrgtAccAngle * 0.001)) ){
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;									// 目標角速度
				}
				else{
					f_TrgtAngleS = f_LastAngleS;
				}
			}
			break;

		/* const(Turn) */
		case CTRL_CONST_TRUN:
			f_TrgtAngleS =f_BaseAngleS;
			break;

		/* dec(Turn) */
		case CTRL_DEC_TRUN:
			f_TrgtAccAngle += f_JerkAngle*0.001;

			/* CCW */
			if( f_LastAngle > 0 ){
				if(f_BaseAccAngle < 0){
					if(f_TrgtAccAngle > 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle < f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				/* Angle speed CTRL + Angle CTRL */
				if( f_TrgtAngleS > (f_LastAngleS +(f_TrgtAccAngle * 0.001)) ){						// 減速目標更新区間
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;							// 目標角速度
					f_TrgtAngle  += f_TrgtAngleS * 0.001;	// 目標角度
				}
				/* Angle CTRL */
				else{
					f_TrgtAngleS = f_LastAngleS;
					f_TrgtAngle  = f_LastAngle;													// 目標距離
				}
			}
			/* CW */
			else{
				if(f_BaseAccAngle > 0){
					if(f_TrgtAccAngle < 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle > f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				/* Angle speed CTRL + Angle CTRL */
				if( f_TrgtAngleS < (f_LastAngleS +(f_TrgtAccAngle * 0.001))){						// 減速目標更新区間
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;							// 目標角速度
					f_TrgtAngle  += f_TrgtAngleS * 0.001;	// 目標角度
				}
				/* Angle CTRL */
				else{
					f_TrgtAngleS = f_LastAngleS;
					f_TrgtAngle  = f_LastAngle;													// 目標距離
				}
			}
			break;

		/* entry(sla) */
		case CTRL_ENTRY_SLA:
			f_TrgtSpeed = f_BaseSpeed;
			if( f_TrgtDist <= f_LastDist - (f_TrgtSpeed * 0.001) ){
				f_TrgtDist  += f_TrgtSpeed * 0.001;								// 目標距離
			}
			break;

		/* acc(スラローム) */
		case CTRL_ACC_SLA:
			f_TrgtSpeed = f_BaseSpeed;
			f_TrgtAccAngle += f_JerkAngle*0.001;

			/* CCW */
			if( f_LastAngle > 0 ){
				if(f_BaseAccAngle > 0){
					if(f_TrgtAccAngle < 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle > f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				if( f_TrgtAngleS < (f_LastAngleS +(f_TrgtAccAngle * 0.001))){
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;							// 目標角速度
					f_TrgtAngle  += f_TrgtAngleS * 0.001;	// 目標角度
//					printf("%5.2f %5.2f %5.4f %5.2f %5.2f\n\r",f_TrgtAngleS,f_AccAngleS,f_Time,f_TrgtAngle,f_LastAngleS);
				}
				else{
					f_TrgtAngle  = f_LastAngle;													// 目標距離
				}
			}
			/* CW */
			else{
				if(f_BaseAccAngle < 0){
					if(f_TrgtAccAngle > 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle < f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				if( f_TrgtAngleS > (f_LastAngleS -(f_TrgtAccAngle * 0.001)) ){
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;							// 目標角速度
					f_TrgtAngle  += f_TrgtAngleS * 0.001;	// 目標角度
//					printf("%5.2f %5.2f %5.4f %5.2f %5.2f\n\r",f_TrgtAngleS,f_AccAngleS,f_Time,f_TrgtAngle,f_LastAngleS);
				}
				else{
					f_TrgtAngle  = f_LastAngle;													// 目標距離
				}
			}

			/* Position CTRL */
			if( f_LastDist > (f_TrgtDist - (f_TrgtSpeed * 0.001)) ){						// 目標更新区間
				f_TrgtDist  += f_TrgtSpeed * 0.001;							// 目標位置
			}
			else{
				f_TrgtDist  = f_LastDist;													// 目標距離
			}
			break;

		/* const(sla) */
		case CTRL_CONST_SLA:
			f_TrgtSpeed = f_BaseSpeed;
			f_TrgtAngleS = f_BaseAngleS;							// 目標角速度

			/* CCW */
			if( f_LastAngle > 0 ){
				if( f_TrgtAngle < (f_LastAngleS +(f_TrgtAccAngle * 0.001)) ){
					f_TrgtAngle	+= f_TrgtAngleS * 0.001;			// 目標角度
				}
				else{
					f_TrgtAngle  = f_LastAngle;									// 目標角度
				}
			}
			/* CW */
			else{
				if( f_TrgtAngle > (f_LastAngleS -(f_TrgtAccAngle * 0.001)) ){
					f_TrgtAngle	+=f_TrgtAngleS * 0.001;			// 目標角度
				}
				else{
					f_TrgtAngle  = f_LastAngle;									// 目標角度
				}
			}

			/* Position CTRL */
			if( f_LastDist > (f_TrgtDist - (f_TrgtSpeed * 0.001)) ){						// 目標更新区間
				f_TrgtDist  += f_TrgtSpeed * 0.001;							// 目標位置
			}
			else{
				f_TrgtDist  = f_LastDist;													// 目標距離
			}
			break;

		/* dec(sla) */
		case CTRL_DEC_SLA:
			f_TrgtSpeed = f_BaseSpeed;
			f_TrgtAccAngle += f_JerkAngle*0.001;

			/* CCW */
			if( f_LastAngle > 0 ){
				if(f_BaseAccAngle < 0){
					if(f_TrgtAccAngle > 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle < f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				if( f_TrgtAngleS > (f_LastAngleS -(f_TrgtAccAngle * 0.001)) ){
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;							// 目標角速度
					f_TrgtAngle  += f_TrgtAngleS * 0.001;	// 目標角度
				}
				else{
					f_TrgtAngleS = 0.0;
					f_TrgtAngle  = f_LastAngle;													// 目標距離
				}
			}
			/*CW*/
			else{
				if(f_BaseAccAngle > 0){
					if(f_TrgtAccAngle < 0.0){
						f_TrgtAccAngle = 0.0;
					}
				}else{
					if(f_TrgtAccAngle > f_LastAccAngle){
						f_TrgtAccAngle = f_LastAccAngle;
					}
				}

				if( f_TrgtAngleS < (f_LastAngleS +(f_TrgtAccAngle * 0.001)) ){
					f_TrgtAngleS += f_TrgtAccAngle * 0.001;							// 目標角速度
					f_TrgtAngle  += f_TrgtAngleS * 0.001;	// 目標角度
				}
				else{
					f_TrgtAngleS = 0.0;
					f_TrgtAngle  = f_LastAngle;													// 目標距離
				}
			}

			/* Position CTRL */
			if( f_LastDist > (f_TrgtDist - (f_TrgtSpeed * 0.001)) ){						// 目標更新区間
				f_TrgtDist  += f_TrgtSpeed * 0.001;							// 目標位置
			}
			else{
				f_TrgtDist  = f_LastDist;													// 目標距離
			}
			break;

		/* escape(sla) */
		case CTRL_EXIT_SLA:
			f_TrgtSpeed = f_BaseSpeed;
			f_TrgtAngleS = 0;
			if( f_TrgtDist <= (f_LastDist -f_TrgtSpeed * 0.001)){
				f_TrgtDist  += f_TrgtSpeed * 0.001;								// 目標距離
			}
			else{
				f_TrgtDist  = f_LastDist;														// 目標距離
			}
			break;

		/* etc */
		default:
			break;
	}
}

enPARAM_MODE Chg_ParamID( enCTRL_TYPE en_type )
{
	switch( en_type ){
		case CTRL_ACC:			return PARAM_ACC;				// 加速中(直進)
		case CTRL_CONST:		return PARAM_CONST;				// 等速中(直進)
		case CTRL_DEC:			return PARAM_DEC;				// 減速中(直進)
		case CTRL_HIT_WALL:		return PARAM_HIT_WALL;			// 壁あて制御
//		case DCMC_BACK_ACC:		return PARAM_BACK_ACC;			// 加速中(後進)
//		case DCMC_BACK_CONST:		return PARAM_BACK_CONST;		// 等速中(後進)
//		case DCMC_BACK_DEC:		return PARAM_BACK_DEC;			// 減速中(後進)
		case CTRL_SKEW_ACC:		return PARAM_SKEW_ACC;			// 加速中(直進)
		case CTRL_SKEW_CONST:		return PARAM_SKEW_CONST;		// 等速中(直進)
		case CTRL_SKEW_DEC:		return PARAM_SKEW_DEC;			// 減速中(直進)
		case CTRL_ACC_TRUN:		return PARAM_ACC_TRUN;			// 加速中(超地信旋回)
		case CTRL_CONST_TRUN:		return PARAM_CONST_TRUN;		// 等速中(超地信旋回)
		case CTRL_DEC_TRUN:		return PARAM_DEC_TRUN;			// 減速中(超地信旋回)
		case CTRL_ENTRY_SLA:		return PARAM_ENTRY_SLA;		// スラローム前の前進動作(スラローム)
		case CTRL_ACC_SLA:		return PARAM_ACC_SLA;			// 加速中(スラローム)
		case CTRL_CONST_SLA:		return PARAM_CONST_SLA;		// 等速中(スラローム)
		case CTRL_DEC_SLA:		return PARAM_DEC_SLA;			// 減速中(スラローム)
		case CTRL_EXIT_SLA:		return PARAM_EXIT_SLA;			// スラローム後の前進動作(スラローム)
		default:			return PARAM_NC;
	}
}

void CTRL_getFF_speed( float* p_err )
{
	/* 動作モードに応じる */
	switch( en_Type ){

		// 加速 
		case CTRL_ACC:
		case CTRL_SKEW_ACC:
		case CTRL_ACC_TRUN:
		case CTRL_ACC_SLA:
			*p_err = f_TrgtAcc;
			break;

		case CTRL_CONST:
		case CTRL_SKEW_CONST:
		case CTRL_CONST_TRUN:
		case CTRL_ENTRY_SLA:
		case CTRL_EXIT_SLA:
		case CTRL_CONST_SLA:
			*p_err = 0;
			break;

		case CTRL_DEC:
		case CTRL_SKEW_DEC:
		case CTRL_DEC_TRUN:
		case CTRL_DEC_SLA:
		case CTRL_HIT_WALL:
			*p_err = f_TrgtAcc;
			break;

		// 加速以外 
		default:
			*p_err = 0;
			break;										// 何もしない
	}

}

void CTRL_getFF_angle( float* p_err )
{
	/* 動作モードに応じる */
	switch( en_Type ){

		// 加速 
		case CTRL_ACC:
		case CTRL_SKEW_ACC:
		case CTRL_ACC_TRUN:
		case CTRL_ACC_SLA:
			*p_err =FABS(f_TrgtAccAngle);
			break;

		case CTRL_CONST:
		case CTRL_SKEW_CONST:
		case CTRL_CONST_TRUN:
		case CTRL_ENTRY_SLA:
		case CTRL_EXIT_SLA:
		case CTRL_CONST_SLA:
			*p_err = 0;
			break;

		case CTRL_DEC:
		case CTRL_SKEW_DEC:
		case CTRL_DEC_TRUN:
		case CTRL_DEC_SLA:
			*p_err = FABS(f_TrgtAccAngle) *(-1.0);
			break;

		// 加速以外 
		default:
			*p_err = 0;
			break;										// 何もしない
	}

}

void CTRL_getSpeedFB( float* p_err )
{
	float		f_speedErr;					// [速度制御] 速度偏差
	float		f_kp = 0.0f;
	float		f_ki = 0.0f;
	float		f_kd = 0.0f;
	/* 速度制御 */
	f_speedErr  = f_TrgtSpeed - f_NowSpeed;					// 速度偏差[mm/s]
/*	f_kp = f_FB_speed_kp;
	f_ki = f_FB_speed_ki;
	f_kd = f_FB_speed_kd;
*/
	f_kp = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_speed_kp;
	f_ki = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_speed_ki;
	f_kd = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_speed_kd;
	/* I成分演算 */
	f_SpeedErrSum += f_speedErr;// * f_ki;			// I成分更新
	if( f_SpeedErrSum > 10.0 ){
		f_SpeedErrSum = 10.0;			// 上限リミッター
	}

	*p_err = f_speedErr * f_kp + f_SpeedErrSum* f_ki + ( f_speedErr - f_ErrSpeedBuf ) * f_kd;				// PI制御量算出

	f_ErrSpeedBuf = f_speedErr;		// 偏差をバッファリング	
/*	if((f_speedErr>1.5)||(f_speedErr<-1.5)){
		Failsafe_flag();
	}
*/
}

void CTRL_getAngleSpeedFB( float* p_err )
{
	float f_err;					// [入力] ジャイロセンサーエラー値､
	float f_kp = 0.0f;				// 比例ゲイン
	float f_ki = 0.0f;
	float f_kd = 0.0f;


	f_err = f_TrgtAngleS - GYRO_getSpeedErr();			// 目標角度 - ジャイロセンサ[deg/s]
/*	f_kp = f_FB_angleS_kp;
	f_ki = f_FB_angleS_ki;
	f_kd = f_FB_angleS_kd;
*/
	f_kp = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_angleS_kp;
	f_ki = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_angleS_ki;
	f_kd = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_angleS_kd;

	f_AngleSErrSum += f_err;//*f_ki;

	if(f_AngleSErrSum > 50.0){
		f_AngleSErrSum = 50.0;			//上限リミッター
	}
	else if(f_AngleSErrSum <-50.0){
		f_AngleSErrSum = -50.0;
	}

	*p_err = f_err * f_kp + f_AngleSErrSum*f_ki + ( f_err - f_ErrAngleSBuf ) * f_kd;		// PID制御

	f_ErrAngleSBuf = f_err;		// 偏差をバッファリング	
	if((f_err>30.0)||(f_err<-30.0)){
		if(!(en_Type == CTRL_HIT_WALL)){
			Failsafe_flag();
		}
	}
}

void CTRL_getAngleFB( float* p_err )
{
    float f_err;                    // [入力] 角度偏差[deg]
    float f_kp = 0.0f;              // 比例ゲイン
    float f_ki = 0.0f;

    f_err = f_TrgtAngle - GYRO_getNowAngle();          // 現在角度[deg]
/*    f_kp = f_FB_angle_kp;
    f_ki = f_FB_angle_ki;
*/
	f_kp = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_angle_kp;
	f_ki = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_angle_ki;

    f_AngleErrSum += f_err;//*f_ki;
    if(f_AngleErrSum > 100.0){
        f_AngleErrSum = 100.0;           //上限リミッター
    }
    else if(f_AngleErrSum <-100.0){
        f_AngleErrSum = -100.0;
    }
    *p_err = f_err * f_kp + f_AngleErrSum*f_ki;        // PID制御｡

}


void CTRL_getSenFB( float* p_err )
{
	float f_err 	= 0;
	float f_kp 		= 0.0f;				// 比例ゲイン
	float f_kd 		= 0.0f;				// 微分ゲイン
	float gyro		= 0.0f;

	/* 直進時 */
	if( ( en_Type == CTRL_ACC ) || ( en_Type == CTRL_CONST ) || ( en_Type == CTRL_DEC )||
			 ( en_Type == CTRL_ENTRY_SLA ) || ( en_Type == CTRL_EXIT_SLA ) ){
/*
		f_kp = f_FB_wall_kp;
		f_kd = f_FB_wall_kd;
*/
		f_kp = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_wall_kp;
		f_kd = PARAM_getGain(Chg_ParamID(en_Type))->f_FB_wall_kd;

		/* 偏差取得 */
		DIST_getErr( &l_WallErr );
		f_err = (float)l_WallErr;

		/* PD制御 */

		f_ErrDistBuf = f_err;		// 偏差をバッファリング

//		*p_err = (f_err * f_kp + ( f_err - f_ErrDistBuf ) * f_kd)*f_NowSpeed*0.001;		// PD制御
		*p_err = f_err * f_kp + ( f_err - f_ErrDistBuf ) * f_kd;
	}
	else if( ( en_Type == CTRL_SKEW_ACC ) || ( en_Type == CTRL_SKEW_CONST ) || ( en_Type == CTRL_SKEW_DEC ) ){

		DIST_getErrSkew( &l_WallErr );
		f_err = (float)l_WallErr;

//		*p_err = f_err * f_kp + ( f_err - f_ErrDistBuf ) * f_kd;		// PD制御
//		*p_err = f_err*f_NowSpeed*0.001;
		*p_err = f_err;
	}
	else {
		*p_err = 0;
	}

}

void CTRL_get_frontwall_v_FB( float* p_err)
{
	float f_v_err 	= 0;
	float f_omega_err 	= 0;
	float f_v_kp 		= 0.0f;				// 比例ゲイン
	float f_v_ki 		= 0.0f;				// 比例ゲイン
	float f_v_kd 		= 0.0f;				// 微分ゲイン
	float gyro		= 0.0f;

	/* 前壁制御 */
	if( en_Type == CTRL_FRONT_WALL ){

		f_v_kp = FB_FRONT_WALL_V_KP;
		f_v_ki = FB_FRONT_WALL_V_KI;
		f_v_kd = FB_FRONT_WALL_V_KD;

		if( en_Type == CTRL_FRONT_WALL){
			l_FrontSen_vErr = ((L_FRONT_REF+FRONT_WALL_MINUS) - DIST_getNowVal( DIST_SEN_L_FRONT )) + ((R_FRONT_REF+FRONT_WALL_MINUS) - DIST_getNowVal( DIST_SEN_R_FRONT ));
			f_v_err = (float)l_FrontSen_vErr;
	
			/* PD制御 */
			f_ErrFrontSen_vBuf = f_v_err;		// 偏差をバッファリング

			*p_err = f_v_err * f_v_kp + ( f_v_err - f_ErrFrontSen_vBuf ) * f_v_kd;		// PD制御
			if(*p_err < - 0.5)*p_err = -0.5;
			else if(*p_err > 0.5)*p_err = 0.5;
		}
	}

}

void CTRL_get_frontwall_omega_FB( float* p_err)
{
	float f_omega_err 	= 0;
	
	float f_omega_kp 		= 0.0f;				// 比例ゲイン
	float f_omega_ki 		= 0.0f;				// 比例ゲイン
	float f_omega_kd 		= 0.0f;				// 微分ゲイン
	float gyro		= 0.0f;

	/* 前壁制御 */
	if( en_Type == CTRL_FRONT_WALL ){
		f_omega_kp = FB_FRONT_WALL_OMEGA_KP;
		f_omega_ki = FB_FRONT_WALL_OMEGA_KI;
		f_omega_kd = FB_FRONT_WALL_OMEGA_KD;

		if( en_Type == CTRL_FRONT_WALL){	
			l_FrontSen_OmegaErr = (DIST_getNowVal( DIST_SEN_L_FRONT )- (L_FRONT_REF+FRONT_WALL_MINUS)) + 
									((R_FRONT_REF+FRONT_WALL_MINUS) - DIST_getNowVal( DIST_SEN_R_FRONT ));
			if(l_FrontSen_OmegaErr > 500)l_FrontSen_OmegaErr = 500;
			if(l_FrontSen_OmegaErr < -500)l_FrontSen_OmegaErr = -500;
			f_omega_err = (float)l_FrontSen_OmegaErr;
	
			/* PD制御 */
			f_ErrFrontSen_OmegaBuf = f_omega_err;		// 偏差をバッファリング

			*p_err =f_omega_err * f_omega_kp + ( f_omega_err - f_ErrFrontSen_OmegaBuf ) * f_omega_kd;		// PD制御
		}
	}

}

void CTRL_getFloorFriction(float* p_err){
	float tread;
	if(( en_Type == CTRL_ACC_TRUN) || (en_Type == CTRL_CONST_TRUN)||( en_Type == CTRL_DEC_TRUN )){
		tread = TREAD_IMAGIN;
	}else{
		tread = TREAD;
	}
//	*p_err = 0;
/*
	if( ( en_Type == CTRL_ACC_SLA ) || (en_Type == CTRL_CONST_SLA)||( en_Type == CTRL_DEC_SLA ) ){
		if(f_TrgtAngleS<0){
			if(Get_NowAngle() < -0.002)
				*p_err = (-1)*0.38/1000.0 + (-1)*0.43/1000.0+f_TrgtAngleS*FABS(f_TrgtAngleS)*tread/2/PI/2300.0;
	//			*p_err = (-1)*0.35/1000.0 + (-1)*0.45/1000.0+f_TrgtAngleS*tread/2/PI/109.0;
			else
				*p_err = (-1)*0.37/1000.0;
			}
		else if(f_TrgtAngleS>0){
			if(Get_NowAngle() > 0.002)
				*p_err = 0.38/1000.0 + 0.43/1000.0+f_TrgtAngleS*FABS(f_TrgtAngleS)*tread/2/PI/2300.0;
	//			*p_err = 0.35/1000.0 + 0.45/1000.0+f_TrgtAngleS*tread/2/PI/109.0;
			else
				*p_err = 0.37/1000.0;
		}else{
			*p_err = 0;
		}
	}
*/
/*
	if( ( en_Type == CTRL_ACC_SLA ) ){
		if(f_TrgtAngleS<0){
				if((PARAM_getSpeedType( PARAM_SLA ) == PARAM_VERY_SLOW))
					*p_err = (-1.0)*(0.70/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_SLOW)
					*p_err = (-1.0)*(0.70/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_NORMAL)
					*p_err = (-1.0)*(1.0/1000.0+FABS(f_TrgtAngleS)*0.0/1000000.0);
				else
					*p_err = (-1.0)*(0.70/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
			}
		else if(f_TrgtAngleS>0){
				if((PARAM_getSpeedType( PARAM_SLA ) == PARAM_VERY_SLOW))
					*p_err = (0.70/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_SLOW)
					*p_err = (0.70/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_NORMAL)
					*p_err = (0.70/1000.0+FABS(f_TrgtAngleS)*50.0/1000000.0);
				else
					*p_err = (0.70/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
		}else{
			*p_err = 0;
		}
	}

	else if(  (en_Type == CTRL_CONST_SLA)){
		if(f_TrgtAngleS<0){
				if((PARAM_getSpeedType( PARAM_SLA ) == PARAM_VERY_SLOW))
					*p_err = (-1.0)*(0.6/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_SLOW)
					*p_err = (-1.0)*(0.6/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_NORMAL)
					*p_err = (-1.0)*(0.55/1000.0+FABS(f_TrgtAngleS)*0.0/1000000.0);
				else
					*p_err = (-1.0)*(0.6/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
			}
		else if(f_TrgtAngleS>0){
				if((PARAM_getSpeedType( PARAM_SLA ) == PARAM_VERY_SLOW))
					*p_err = (0.6/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_SLOW)
					*p_err = (0.6/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_NORMAL)
					*p_err = (0.6/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
				else
					*p_err = (0.6/1000.0+FABS(f_TrgtAngleS)*80.0/1000000.0);
		}else{
			*p_err = 0;
		}
	}

	else if(( en_Type == CTRL_DEC_SLA )){
		if(f_TrgtAngleS<0){
				if((PARAM_getSpeedType( PARAM_SLA ) == PARAM_VERY_SLOW))
					*p_err = (-1.0)*(0.20/1000.0+FABS(f_TrgtAngleS)*70.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_SLOW)
					*p_err = (-1.0)*(0.20/1000.0+FABS(f_TrgtAngleS)*70.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_NORMAL)
					*p_err = (-1.0)*(0.00/1000.0+FABS(f_TrgtAngleS)*0.0/1000000.0);
				else
					*p_err = (-1.0)*(0.20/1000.0+FABS(f_TrgtAngleS)*70.0/1000000.0);
			}
		else if(f_TrgtAngleS>0){
				if((PARAM_getSpeedType( PARAM_SLA ) == PARAM_VERY_SLOW))
					*p_err = (0.20/1000.0+FABS(f_TrgtAngleS)*70.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_SLOW)
					*p_err = (0.20/1000.0+FABS(f_TrgtAngleS)*70.0/1000000.0);
				else if(PARAM_getSpeedType( PARAM_SLA ) == PARAM_NORMAL)
					*p_err = (0.20/1000.0+FABS(f_TrgtAngleS)*70.0/1000000.0);
				else
					*p_err = (0.20/1000.0+FABS(f_TrgtAngleS)*70.0/1000000.0);
		}else{
			*p_err = 0;
		}
	}
*/
//	else{
		if(f_TrgtAngleS<0){
/*			if(Get_NowAngle() > -0.002)
				*p_err = (-1.0)*0.43/1000.0 + (-1.0)*0.46/1000.0+f_TrgtAngleS*FABS(f_TrgtAngleS)*tread/2.0/PI/740.0;
	//			*p_err = (-1)*0.35/1000.0 + (-1)*0.45/1000.0+f_TrgtAngleS*tread/2/PI/109.0;
			else
				*p_err = (-1.0)*0.37/1000.0;
			}
*/
			*p_err = 0.0;//(-1.0)*0.2/1000.0;
		}
		else if(f_TrgtAngleS>0){
/*
			if(Get_NowAngle() < 0.002)
				*p_err = 0.43/1000.0 + 0.46/1000.0+f_TrgtAngleS*FABS(f_TrgtAngleS)*tread/2.0/PI/740.0;
	//			*p_err = 0.35/1000.0 + 0.45/1000.0+f_TrgtAngleS*tread/2/PI/109.0;
			else
				*p_err = 0.37/1000.0;
*/
			*p_err = 0.0;//0.2/1000.0;
		}else{
			*p_err = 0;
//		}
	}

/*	if(*p_err>0.0014)
		*p_err = 0.0014;
	if(*p_err<-0.0014)
		*p_err = -0.0014;
*/
}

void CTRL_outMot( float f_duty10_R, float f_duty10_L )
{
	float	f_temp;			// 計算用

	/* 電圧に応じてPWM出力を変更する */
	f_duty10_R = f_duty10_R*1000.0;
	f_duty10_L = f_duty10_L*1000.0;

	/* 右モータ */
	if( 25 < f_duty10_R ){									// 前進
		DCM_setDirCcw( DCM_R );
		DCM_setPwmDuty( DCM_R, (uint16_t)f_duty10_R );
	}
	else if( f_duty10_R < -25 ){							// 後退
		f_temp = f_duty10_R * -1.0;
		DCM_setDirCw( DCM_R );
		DCM_setPwmDuty( DCM_R, (uint16_t)f_temp );
	}
	else{
		DCM_brakeMot( DCM_R );								// ブレーキ
	}

	/* 左モータ */
	if( 25 < f_duty10_L ){									// 前進
		DCM_setDirCw( DCM_L );
		DCM_setPwmDuty( DCM_L, (uint16_t)f_duty10_L );
	}
	else if( f_duty10_L < -25 ){							// 後退
		f_temp = f_duty10_L * -1.0;
		DCM_setDirCcw( DCM_L );
		DCM_setPwmDuty( DCM_L, (uint16_t)f_temp );
	}
	else{
		DCM_brakeMot( DCM_L );								// ブレーキ
	}
}

void CTRL_pol( void )
{
	float f_feedFoard_speed		= 0;		// [制御] フィードフォワード速度制御
	float f_feedFoard_angle		= 0;		// [制御] フィードフォワード角度制御
	float f_speedCtrl			= 0;		// [制御] 速度制御量
	float f_angleSpeedCtrl			= 0;	// [制御] 角速度制御量
	float f_angleCtrl			= 0;		// [制御] 角度制御量
	float f_distSenCtrl			= 0;		// [制御] 距離センサー制御量
	float f_frontwall_v_Ctrl		= 0;
	float f_frontwall_omega_Ctrl	= 0;
	float f_floorfriction		= 0;
	float f_duty10_R;						// [出力] 右モータPWM-DUTY比[0.1%]
	float f_duty10_L;						// [出力] 左モータPWM-DUTY比[0.1%]

	float TR = 0.0;
	float TL = 0.0;
	float Ir = 0.0;
	float Il = 0.0;

	ENC_GetDiv( &l_CntR, &l_CntL );					// 移動量[カウント値]を取得
	//add get_motor_omega(l_CntR,l_CntL);
	CTRL_refNow();									// 制御に使用する値を現在の状態に更新
	CTRL_refTarget();								// 制御に使用する値を目標値に更新

	/* 制御を行うかのチェック */
	if( uc_CtrlFlag != TRUE ){
		 return;		// 制御無効状態
	}
	if(SW_ON == SW_IsOn_0()){
		Failsafe_flag();
	}

	/* 制御不能 */
	if (SYS_isOutOfCtrl() == TRUE ){

		f_DistErrSum = 0;				// 累積偏差クリア
		f_NowDist = f_LastDist;			// 強制的に最終目標位置に変更
		f_NowAngle = f_LastAngle;		// 強制的に最終目標時間に変更
		f_Time = f_TrgtTime;			// 強制的に最終目標時間に変更

	 	CTRL_stop();				// 制御停止
		CTRL_clrData();					// データクリア
		DCM_brakeMot( DCM_R );			// ブレーキ
		DCM_brakeMot( DCM_L );			// ブレーキ
	}

	/* 各種センサ入力 */
/*	ENC_GetDiv( &l_CntR, &l_CntL );					// 移動量[カウント値]を取得
	//add get_motor_omega(l_CntR,l_CntL);
	CTRL_refNow();									// 制御に使用する値を現在の状態に更新
	CTRL_refTarget();								// 制御に使用する値を目標値に更新
*/
	f_NowAngle = GYRO_getNowAngle();					// 現在角度[deg]

	/* 制御値取得 */
	CTRL_getFF_speed( &f_feedFoard_speed );					// [制御] フィードフォワード
	CTRL_getFF_angle( &f_feedFoard_angle );					// [制御] フィードフォワード
	CTRL_getSpeedFB( &f_speedCtrl );				// [制御] 速度
	CTRL_getAngleSpeedFB( &f_angleSpeedCtrl );		// [制御] 角速度
	CTRL_getAngleFB( &f_angleCtrl );		//angle ctrl
	CTRL_getSenFB( &f_distSenCtrl );				// [制御] 壁
	CTRL_getFloorFriction( &f_floorfriction );
	CTRL_get_frontwall_v_FB( &f_frontwall_v_Ctrl);
	CTRL_get_frontwall_omega_FB( &f_frontwall_omega_Ctrl);

	/* 直進制御 */
	if( ( en_Type == CTRL_ACC ) || ( en_Type == CTRL_CONST ) || ( en_Type == CTRL_DEC ) ||( en_Type == CTRL_ENTRY_SLA ) || ( en_Type == CTRL_EXIT_SLA ) ||
		( en_Type == CTRL_SKEW_ACC ) || ( en_Type == CTRL_SKEW_CONST ) || ( en_Type == CTRL_SKEW_DEC )
	){
		TR = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)+(TIRE_D/2.0/TREAD)*(INERTIA*(f_feedFoard_angle + f_angleSpeedCtrl+ f_distSenCtrl)))/GEAR_RATIO;
		TL = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)-(TIRE_D/2.0/TREAD)*(INERTIA*(f_feedFoard_angle + f_angleSpeedCtrl+ f_distSenCtrl)))/GEAR_RATIO;
		Ir = (TR+0.0/1000.0)/TORQUE_CONSTANT;
		Il = (TL+0.0/1000.0)/TORQUE_CONSTANT;
	}

	/* 壁あて制御 */
	else if( en_Type == CTRL_HIT_WALL ){
		TR = (TIRE_D/2.0/2.0)*(WEIGHT*(-1.0)*(300.0 * FF_HIT_BALANCE_R/1000.0 ));		
		TL = (TIRE_D/2.0/2.0)*(WEIGHT*(-1.0)*(300.0 * FF_HIT_BALANCE_R/1000.0 ));
		Ir = (TR-0.0255/1000.0)/TORQUE_CONSTANT;
		Il = (TL-0.0255/1000.0)/TORQUE_CONSTANT;
	}

	/* スラローム制御 */
	else if( ( en_Type == CTRL_ACC_SLA ) || (en_Type == CTRL_CONST_SLA)||( en_Type == CTRL_DEC_SLA ) ){
		/* 左旋回 */
		if( f_LastAngle > 0 ){
			TR = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)+(TIRE_D/2.0/TREAD)*(INERTIA*(f_feedFoard_angle + f_angleSpeedCtrl+f_angleCtrl)/*+f_floorfriction*/))/GEAR_RATIO;
			TL = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)-(TIRE_D/2.0/TREAD)*(INERTIA*(f_feedFoard_angle + f_angleSpeedCtrl+f_angleCtrl)/*+f_floorfriction*/))/GEAR_RATIO;
			Ir = (TR+0.0255/1000.0)/TORQUE_CONSTANT;
			Il = (TL-0.0255/1000.0)/TORQUE_CONSTANT;
		}
		/*右旋回 */
		else{			
			TR = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)+(TIRE_D/2.0/TREAD)*(INERTIA*(f_feedFoard_angle*(-1.0) + f_angleSpeedCtrl+f_angleCtrl)/*+f_floorfriction*/))/GEAR_RATIO;
			TL = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)-(TIRE_D/2.0/TREAD)*(INERTIA*(f_feedFoard_angle*(-1.0) + f_angleSpeedCtrl+f_angleCtrl)/*+f_floorfriction*/))/GEAR_RATIO;
			Ir = (TR-0.0255/1000.0)/TORQUE_CONSTANT;
			Il = (TL+0.0255/1000.0)/TORQUE_CONSTANT;
		}
	}

	else if( en_Type == CTRL_FRONT_WALL){
		TR = ((TIRE_D/2.0/2.0)*(WEIGHT*f_frontwall_v_Ctrl)+(TIRE_D/2.0/TREAD)*(INERTIA*f_frontwall_omega_Ctrl))/GEAR_RATIO;
		TL = ((TIRE_D/2.0/2.0)*(WEIGHT*f_frontwall_v_Ctrl)-(TIRE_D/2.0/TREAD)*(INERTIA*f_frontwall_omega_Ctrl))/GEAR_RATIO;
		Ir = (TR)/TORQUE_CONSTANT;
		Il = (TL)/TORQUE_CONSTANT;
	}

	/* 超信地旋回 */
	else{
		/* 左旋回 */
		if( f_LastAngle > 0 ){			
			TR = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)+(TIRE_D/2.0/TREAD_IMAGIN)*(INERTIA*(f_feedFoard_angle + f_angleSpeedCtrl+f_angleCtrl)+f_floorfriction))/GEAR_RATIO;
			TL = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)-(TIRE_D/2.0/TREAD_IMAGIN)*(INERTIA*(f_feedFoard_angle + f_angleSpeedCtrl+f_angleCtrl)+f_floorfriction))/GEAR_RATIO;
			Ir = (TR+0.01/1000.0)/TORQUE_CONSTANT;
			Il = (TL-0.01/1000.0)/TORQUE_CONSTANT;
		}
		/* 右旋回 */
		else{			
			TR = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)+(TIRE_D/2.0/TREAD_IMAGIN)*(INERTIA*(f_feedFoard_angle*(-1.0) + f_angleSpeedCtrl+f_angleCtrl)+f_floorfriction))/GEAR_RATIO;
			TL = ((TIRE_D/2.0/2.0)*((WEIGHT*(f_feedFoard_speed + f_speedCtrl))+0.0)-(TIRE_D/2.0/TREAD_IMAGIN)*(INERTIA*(f_feedFoard_angle*(-1.0) + f_angleSpeedCtrl+f_angleCtrl)+f_floorfriction))/GEAR_RATIO;
			Ir = (TR-0.01/1000.0)/TORQUE_CONSTANT;
			Il = (TL+0.01/1000.0)/TORQUE_CONSTANT;
		}
	}
	f_duty10_R = FF_BALANCE_R*(MOTOR_REGISTER*Ir+f_MotorR_AngleS*60.0/(2.0*PI)* TORQUE_CONSTANT*(PI*2.0)/60.0)/get_battLv();	
	f_duty10_L = FF_BALANCE_L*(MOTOR_REGISTER*Il+f_MotorL_AngleS*60.0/(2.0*PI)* TORQUE_CONSTANT*(PI*2.0)/60.0)/get_battLv();	

	if(f_duty10_R>1){
		f_duty10_R = 1.0;
	}
	if(f_duty10_R<-1){
		f_duty10_R = -1.0;
	}
	if(f_duty10_L>1){
		f_duty10_L = 1.0;
	}
	if(f_duty10_L<-1){
		f_duty10_L = -1.0;
	}

	Duty_L = f_duty10_L;
	Duty_R = f_duty10_R;

	TempLog1 = TL*1000.0;//f_AngleSErrSum;//TR;//f_floorfriction;//f_duty10_R;
	TempLog2 = f_feedFoard_speed;//f_angleSpeedCtrl;//TL;//f_duty10_L;
	TempLog3 = f_speedCtrl;//f_floorfriction;//f_feedFoard_angle*(-1.0);
	TempLog4 = f_duty10_L;//f_SpeedErrSum;//f_floorfriction;//INERTIA*(f_feedFoard_angle*(-1.0) + f_angleSpeedCtrl+f_angleCtrl)+f_floorfriction * 1000000.0;

	EscapeWait = EscapeWait+0.001;
	CTRL_outMot( f_duty10_R, f_duty10_L );				// モータへ出力

	f_Time += 0.001;

	/* 壁切れチェック */
	if( MOT_getWallEdgeType() == MOT_WALL_EDGE_RIGHT ){

		/* 壁抜け */
		if( DIST_isWall_R_SIDE() == FALSE ){
			SetLED(0x10);
			MOT_setWallEdge( TRUE );		// 壁の切れ目を検知
		}
	}
	else if( MOT_getWallEdgeType() == MOT_WALL_EDGE_LEFT ){

		/* 壁抜け */
		if( DIST_isWall_L_SIDE() == FALSE ){
			SetLED(0x01);
			MOT_setWallEdge( TRUE );		// 壁の切れ目を検知
		}
	}
}

void Failsafe_flag(void)
{
	bl_Failsafe = TRUE;
	SetLED(0x1F);
}

void Failsafe_flag_off(void)
{
	bl_Failsafe = FALSE;
}

bool SYS_isOutOfCtrl( void )
{
	return bl_Failsafe;
}


