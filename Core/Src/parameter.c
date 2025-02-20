/*
 * parameter.c
 *
 *  Created on: 2021/02/04
 *      Author: shohe
 */

#include "parameter.h"

#define GET_INDEX_ST(i)			( i - PARAM_ST_TOP - 1 )		
#define GET_INDEX_TRUN(i)		( i - PARAM_TRUN_TOP - 1 )		
#define GET_INDEX_SLA(i)		( i - PARAM_SLA_TOP - 1 )	

volatile uint16_t		Msec_in;
volatile uint8_t		Sec_in;	
volatile uint8_t		Min_in;	

enPARAM_MOVE_SPEED		en_Speed_st	= PARAM_NORMAL;			// 直進時の移動速度タイプ
enPARAM_MOVE_SPEED		en_Speed_trun	= PARAM_NORMAL;		// 旋回時の移動速度タイプ
enPARAM_MOVE_SPEED		en_Speed_sla	= PARAM_NORMAL;		// スラローム時の移動速度タイプ
stSLA				st_Sla[SLA_TYPE_MAX];					// スラローム時の走行パラメータ

/* ============ */
/*  速度データ  */
/* ============ */

	/* 直進速度データ */
	const stSPEED f_StSpeedData[PARAM_MOVE_SPEED_MAX] = {
		
		//	ジャーク　加速度		減速度		角ジャーク	角加速度		角減速度
		{ 200.0,	4.5,			4.5,		0,		0,				0,				},		// 超低速(PARAM_VERY_SLOW)
		{ 200.0,	4.5,			4.5,		0,		0,				0,				},		// 低速(PARAM_SLOW)
		{ 200.0,	4.5,			4.5,		0,		0,				0,				},		// 通常(PARAM_NORMAL)
		{ 500.0,	10.0,			10.0,		0,		0,				0,				},		// 高速(PARAM_FAST)
		{ 500.0,	10.0,			10.0,		0,		0,				0,				},		// 超高速(PARAM_VERY_FAST)
	};

	/* 旋回速度データ */
	const stSPEED f_TurnSpeedData[PARAM_MOVE_SPEED_MAX] = {
		
		//	ジャーク　加速度	減速度		角ジャーク	角加速度		角減速度
		{ 0,		0,			0,			500.0*PI,		22.2*PI,		22.2*PI,		},		// 超低速(PARAM_VERY_SLOW)
		{ 0,		0,			0,			500.0*PI,		22.2*PI,		22.2*PI,		},		// 低速(PARAM_SLOW)
		{ 0,		0,			0,			500.0*PI,		22.2*PI,		22.2*PI,		},		// 通常(PARAM_NORMAL)
		{ 0,		0,			0,			500.0*PI,		22.2*PI,		22.2*PI,		},		// 高速(PARAM_FAST)
		{ 0,		0,			0,			500.0*PI,		22.2*PI,		22.2*PI,		},		// 超高速(PARAM_VERY_FAST)
	};

	/* スラローム速度データ (結局つかってないか)*/
	const stSPEED f_SlaSpeedData[PARAM_MOVE_SPEED_MAX] = {
		
		//	ジャーク　加速度	減速度		角ジャーク	角加速度		角減速度
		{ 0,	4.5,			4.5,		3000.0*PI,		10.0*PI,			10.0*PI,			},		// 超低速(PARAM_VERY_SLOW)
		{ 0,	4.5,			4.5,		3000.0*PI,		10.0*PI,			10.0*PI,			},		// 低速(PARAM_SLOW)
		{ 0,	4.5,			4.5,		3000.0*PI,		10.0*PI,			10.0*PI,			},		// 通常(PARAM_NORMAL)
		{ 0,	4.5,			4.5,		3000.0*PI,		10.0*PI,			10.0*PI,			},		// 高速(PARAM_FAST)
		{ 0,	4.5,			4.5,		3000.0*PI,		10.0*PI,			10.0*PI,			},		// 超高速(PARAM_VERY_FAST)
	};


/* ============== */
/*  GainData  */
/* ============== */


	/* 直進ゲインデータ */
	const stGAIN f_StGainData[PARAM_MOVE_SPEED_MAX] = {
		
		/* 超低速(PARAM_VERY_SLOW) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 10.0,4.0,0.1, 15.0,0.0,0.0,	0.0,0.0,	0.0,0.0,},
		/* 低速(PARAM_SLOW) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	155.0,0.2,0.03,	0.0,0.0,	0.6,0.1,},
		/* 通常(PARAM_NORMAL) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 35.0,4.0,0.1,	155.0,0.2,0.03,	0.0,0.0,	0.6,0.1,},
		/* 高速(PARAM_FAST) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 35.0,4.0,0.1,	155.0,0.2,0.03,	0.0,0.0,	0.6,0.1,},
		/* 超高速(PARAM_VERY_FAST) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 35.0,4.0,0.1,	155.0,0.2,0.03,	0.0,0.0,	0.6,0.1,}
	};

	/* 旋回ゲインデータ */
	const stGAIN f_TurnGainData[PARAM_MOVE_SPEED_MAX] = {
		
		/* 超低速(PARAM_VERY_SLOW) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 10.0,4.0,0.1,	70.0,2.0,0.5,	0.0,0.0,	0.0,0.0,},
		/* 低速(PARAM_SLOW) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	200.0,15.0,3.0,	0.0,0.0,	0.0,0.0,},
		/* 通常(PARAM_NORMAL) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	200.0,15.0,3.0,	0.0,0.0,	0.0,0.0,},
		/* 高速(PARAM_FAST) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	200.0,15.0,3.0,	0.0,0.0,	0.0,0.0,},
		/* 超高速(PARAM_VERY_FAST) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	200.0,15.0,3.0,	0.0,0.0,	0.0,0.0,}
	};

	/* スラロームゲインデータ */
	const stGAIN f_SlaGainData[PARAM_MOVE_SPEED_MAX] = {

		/* 超低速(PARAM_VERY_SLOW) *///吸引なし　300
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 10.0,4.0,0.0,	20.0,0.0,0.0,	0.0,0.0,	0.6,0.1,},
		/* 低速(PARAM_SLOW) *///吸引あり300
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	240.0,8.0,0.5,	0.0,0.0,	0.6,0.1,},
		/* 通常(PARAM_NORMAL)*///吸引あり500
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	240.0,8.0,0.5,	0.0,0.0,	0.6,0.1,},
		/* 高速(PARAM_FAST)*/
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	240.0,8.0,0.5,	0.0,0.0,	0.6,0.1,},
		/* 超高速(PARAM_VERY_FAST) */
			//速度kp,速度ki,速度kd,	角速度kp,角速度ki,角速度kd,	角度kp,角度ki,	壁kp,壁kd
			{ 30.0,4.0,0.1,	240.0,8.0,0.5,	0.0,0.0,	0.6,0.1,}
	};

const stGAIN* PARAM_getGain( enPARAM_MODE en_mode )
{
	const stGAIN* p_adr;
	
	switch( en_mode ){
		
		case PARAM_ACC:													// 加速中(直進)
		case PARAM_CONST:												// 等速中(直進)
		case PARAM_DEC:													// 減速中(直進)
//		case PARAM_BACK_ACC:											// 加速中(後進)
//		case PARAM_BACK_CONST:											// 等速中(後進)
//		case PARAM_BACK_DEC:											// 減速中(後進)
		case PARAM_SKEW_ACC:											// 加速中(斜め)
		case PARAM_SKEW_CONST:											// 等速中(斜め)
		case PARAM_SKEW_DEC:											// 減速中(斜め)
		case PARAM_HIT_WALL:											// 壁あて制御
			p_adr = &f_StGainData[en_Speed_st];
			break;
			
		case PARAM_ACC_TRUN:											// 加速中(超地信旋回)
		case PARAM_CONST_TRUN:											// 等速中(超地信旋回)
		case PARAM_DEC_TRUN:											// 減速中(超地信旋回)
			p_adr = &f_TurnGainData[en_Speed_trun];
			break;
			
		case PARAM_ENTRY_SLA:											// スラローム前の前進動作(スラローム)
		case PARAM_ACC_SLA:											// 加速中(スラローム)
		case PARAM_CONST_SLA:											// 等速中(スラローム)
		case PARAM_DEC_SLA:											// 減速中(スラローム)
		case PARAM_EXIT_SLA:											// スラローム後の前進動作(スラローム)
			p_adr = &f_SlaGainData[en_Speed_sla];
			break;
		
		default:														// Err、とりあえず・・・（メモリ破壊を防ぐため）
			printf("設定したゲインタイプがありません \n\r");
			p_adr = &f_SlaGainData[en_Speed_sla];
			break;
	}
	
	return p_adr;
}

enPARAM_MOVE_SPEED PARAM_getSpeedType( enPARAM_MODE en_mode )
{
	switch( en_mode ){
		
		case PARAM_ST:
			return en_Speed_st;
			break;
		
		case PARAM_TRUN:
			return en_Speed_trun;
			break;
		
		case PARAM_SLA:
			return en_Speed_sla;
			break;
			
		default:
			printf("Can't find parameter type \n\r");
			return 0;
			break;
	}
}

void PARAM_setSpeedType( enPARAM_MODE en_mode, enPARAM_MOVE_SPEED en_speed )
{
	switch( en_mode ){
		
		case PARAM_ST:
			en_Speed_st = en_speed;
			break;
		
		case PARAM_TRUN:
			en_Speed_trun = en_speed;
			break;
		
		case PARAM_SLA:
			en_Speed_sla = en_speed;
			break;
			
		default:
			printf("Can't find parameter type \n\r");
			break;
	}
}

const stSPEED* PARAM_getSpeed( enPARAM_MODE en_mode )
{
	const stSPEED* p_adr;
	
	switch( en_mode ){
		
		case PARAM_ST:													// 直進
		case PARAM_ACC:													// 加速中(直進)
		case PARAM_CONST:												// 等速中(直進)
		case PARAM_DEC:													// 減速中(直進)
//		case PARAM_BACK_ACC:											// 加速中(後進)
//		case PARAM_BACK_CONST:											// 等速中(後進)
//		case PARAM_BACK_DEC:											// 減速中(後進)
		case PARAM_SKEW_ACC:											// 加速中(斜め)
		case PARAM_SKEW_CONST:											// 等速中(斜め)
		case PARAM_SKEW_DEC:											// 減速中(斜め)
		case PARAM_HIT_WALL:											// 壁あて制御
			p_adr = &f_StSpeedData[en_Speed_st];
			break;
			
		case PARAM_TRUN:												// 旋回
		case PARAM_ACC_TRUN:											// 加速中(超地信旋回)
		case PARAM_CONST_TRUN:											// 等速中(超地信旋回)
		case PARAM_DEC_TRUN:											// 減速中(超地信旋回)
			p_adr = &f_TurnSpeedData[en_Speed_trun];
			break;
			
		case PARAM_SLA:													// スラローム
		case PARAM_ENTRY_SLA:											// スラローム前の前進動作(スラローム)
		case PARAM_ACC_SLA:											// 加速中(スラローム)
		case PARAM_CONST_SLA:											// 等速中(スラローム)
		case PARAM_DEC_SLA:											// 減速中(スラローム)
		case PARAM_EXIT_SLA:											// スラローム後の前進動作(スラローム)
			p_adr = &f_SlaSpeedData[en_Speed_sla];
			break;

		default:														// Err、とりあえず・・・（メモリ破壊を防ぐため）
			printf("Can't find speed type \n\r");
			p_adr = &f_SlaSpeedData[en_Speed_sla];
			break;
	}
	
	return p_adr;
}
/*
const stGAIN* PARAM_getGain( enPARAM_MODE en_mode )
{
	const stGAIN* p_adr;
	
	switch( en_mode ){
		
		case PARAM_ACC:													// 加速中(直進)
		case PARAM_CONST:												// 等速中(直進)
		case PARAM_DEC:													// 減速中(直進)
//		case PARAM_BACK_ACC:											// 加速中(後進)
//		case PARAM_BACK_CONST:											// 等速中(後進)
//		case PARAM_BACK_DEC:											// 減速中(後進)
		case PARAM_SKEW_ACC:											// 加速中(斜め)
		case PARAM_SKEW_CONST:											// 等速中(斜め)
		case PARAM_SKEW_DEC:											// 減速中(斜め)
		case PARAM_HIT_WALL:											// 壁あて制御
			p_adr = &f_StGainData[en_Speed_st][GET_INDEX_ST( en_mode )];
			break;
			
		case PARAM_ACC_TRUN:											// 加速中(超地信旋回)
		case PARAM_CONST_TRUN:											// 等速中(超地信旋回)
		case PARAM_DEC_TRUN:											// 減速中(超地信旋回)
			p_adr = &f_TurnGainData[en_Speed_trun][GET_INDEX_TRUN( en_mode )];
			break;
			
		case PARAM_ENTRY_SLA:											// スラローム前の前進動作(スラローム)
		case PARAM_ACC_SLA:											// 加速中(スラローム)
		case PARAM_CONST_SLA:											// 等速中(スラローム)
		case PARAM_DEC_SLA:											// 減速中(スラローム)
		case PARAM_EXIT_SLA:											// スラローム後の前進動作(スラローム)
			p_adr = &f_SlaGainData[en_Speed_sla][GET_INDEX_SLA( en_mode )];
			break;
		
		default:														// Err、とりあえず・・・（メモリ破壊を防ぐため）
			printf("Can't find gain type \n\r");
			p_adr = &f_SlaGainData[en_Speed_sla][GET_INDEX_SLA( en_mode )];
			break;
	}
	
	return p_adr;
}
*/

void PARAM_makeSra( float f_speed, float f_angAcc, float f_g , float f_jerkAngle, enSLA_TYPE en_mode)
{

	float	f_start_x;					// 開始x位置 [mm]
	float	f_start_y;					// 開始y位置 [mm]
	float	f_final_x;					// 最終x位置 [mm]
	float	f_final_y;					// 最終y位置 [mm]
	float	f_final_ang;				// 角減速時の最終角度 [rad]	
	float	f_maxAngleV		= 0;		// 最大角速度[rad/s]
	float	f_timeAcc		= 0;		// 加速時間[s]
	float	f_accAngle		= 0;		// 加速角度[rad]
	float	f_timeConst		= 0;		// 等速時間[s]
	float	f_constAngle	= 0;		// 等速角度[rad]
	float	f_ang			= 0;		// 演算用、角度 [rad]
	float	f_time			= 0;		// 演算用、時間 [s]
	float	f_x;						// 演算用x位置 [mm]
	float	f_y;						// 演算用y位置 [mm]
	uint16_t	i = 0;						// ループ用
	stSLA* 	p_adr = &st_Sla[en_mode];		// 記録する走行データ

	float	f_jerkAngleTime		= 0;
	float	f_jerkAngleSpeed	= 0;
	float	f_jerkAccAccAngle	= 0;
	float	f_jerkAccDecAngle	= 0;
	float	f_jerkDecDecAngle	= 0;
	float	f_DecConstAngle	= 0;
	float	f_jerkDecAccAngle	= 0;

	// スラロームに応じた設定値からスラロームに必要なパラメータを演算する 
	switch(en_mode){

		case SLA_90:
			f_start_x   = HALF_BLOCK;
			f_start_y   = 0.0f;
			f_final_x   = BLOCK;
			f_final_y   = HALF_BLOCK;
			f_final_ang = 90.0f * DEG_TO_RAD;
			break;

		case SLA_45:
			f_start_x   = HALF_BLOCK;
			f_start_y   = 0.0f;
			f_final_x   = BLOCK * 0.75f;
			f_final_y   = BLOCK * 0.75f;
			f_final_ang = 45.0f * DEG_TO_RAD;
			break;
			
		case SLA_N90:
			f_start_x   = HALF_BLOCK * 0.5f * 1.4142f;
			f_start_y   = 0.0f;
			f_final_x   = HALF_BLOCK * 1.4142f;
			f_final_y   = HALF_BLOCK * 0.5f * 1.4142f;
			f_final_ang = 90.0f * DEG_TO_RAD;
			break;
			
		case SLA_135:
			f_start_x   = HALF_BLOCK;
			f_start_y   = 0.0f;
			f_final_x   = BLOCK * 1.25f;
			f_final_y   = BLOCK * 0.25;
			f_final_ang = 135.0f * DEG_TO_RAD;
			break;

		default:
			break;
	}

	// caluculate acc and dec angle speed 
	f_maxAngleV			= f_g / f_speed;							// max angle speed[rad/s] (omega[rad/s] = g[m/s^2] / v[m/s] )
	f_jerkAngleTime		= f_angAcc/ f_jerkAngle;
	f_jerkAngleSpeed	= 0.5 * f_jerkAngle * f_jerkAngleTime * f_jerkAngleTime;
	f_timeAcc			= (f_maxAngleV- f_jerkAngleSpeed * 2.0) / f_angAcc;					// acc time[s]
	f_jerkAccAccAngle	= 1.0 / 6.0 * f_jerkAngle * f_jerkAngleTime * f_jerkAngleTime * f_jerkAngleTime + 0.0 * f_jerkAngleTime;
	f_accAngle			= 0.5 * f_angAcc * f_timeAcc * f_timeAcc + f_jerkAngleSpeed * f_timeAcc;	// acc angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
	f_jerkAccDecAngle	= 1.0 / 6.0 * f_jerkAngle* (-1.0) * f_jerkAngleTime * f_jerkAngleTime * f_jerkAngleTime 
							+ (f_maxAngleV-f_jerkAngleSpeed) * f_jerkAngleTime + 1.0/2.0*f_angAcc*f_jerkAngleTime*f_jerkAngleTime;
//	f_constAngle		= f_final_ang - (f_jerkAccAccAngle+f_accAngle+f_jerkAccDecAngle)* 2.0;				// const angle[rad] (theta[rad] = Totalangle - (acc angle + dec angle) )
//	f_timeConst			= f_constAngle / f_maxAngleV;				// max angle speed time[s]( t[s] = theta[rad] / omega[rad/s] )
	f_jerkDecDecAngle	= 1.0 / 6.0 * f_jerkAngle* (-1.0) * f_jerkAngleTime * f_jerkAngleTime * f_jerkAngleTime + f_maxAngleV * f_jerkAngleTime ;
	f_DecConstAngle		= 0.5 * f_angAcc*(-1.0) * f_timeAcc * f_timeAcc + (f_maxAngleV-f_jerkAngleSpeed) * f_timeAcc;
	f_jerkDecAccAngle	= 1.0 / 6.0 * f_jerkAngle * f_jerkAngleTime * f_jerkAngleTime * f_jerkAngleTime + f_jerkAngleSpeed * f_jerkAngleTime 
							+ 1.0/2.0*f_angAcc*(-1.0)*f_jerkAngleTime*f_jerkAngleTime;
	f_constAngle		= f_final_ang - (f_jerkAccAccAngle+f_accAngle+f_jerkAccDecAngle +f_jerkDecDecAngle+f_DecConstAngle+f_jerkDecAccAngle) ;				// const angle[rad] (theta[rad] = Totalangle - (acc angle + dec angle) )
	f_timeConst			= f_constAngle / f_maxAngleV;

	// -------------------------------- 
	// sla end position 
	// -------------------------------- 
	// start position 
	f_x		= f_start_x;
	f_y		= f_start_y;

	// jerk ACC acc
	for( i=0; i<(uint16_t)(f_jerkAngleTime*1000); i++ ){
		f_time	=  0.001f * (float)i;								// time[s]
//		f_ang	=  0.5f * f_angAcc * f_time * f_time;				// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_ang	= 1.0 / 6.0 * f_jerkAngle * f_time * f_time * f_time;
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;			// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;			// Yposition[m]
	}
	// ACC 
	for( i=0; i<(uint16_t)(f_timeAcc*1000); i++ ){				// [msec]
	
		f_time	=  0.001f * (float)i;								// time[s]
		f_ang	= f_jerkAccAccAngle + 0.5f * f_angAcc * f_time * f_time+ f_jerkAngleSpeed*f_time;				// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;			// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;			// Yposition[m]
	}
	//jerk ACC dec
	for( i=0; i<(uint16_t)(f_jerkAngleTime*1000); i++ ){
		f_time	=  0.001f * (float)i;								// time[s]
//		f_ang	=  0.5f * f_angAcc * f_time * f_time;				// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_ang	= f_jerkAccAccAngle + f_accAngle + 1.0 / 6.0 * f_jerkAngle * (-1.0) * f_time * f_time * f_time + (f_maxAngleV-f_jerkAngleSpeed) * f_time + 1.0/2.0*f_angAcc*f_time*f_time;
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;			// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;			// Yposition[m]
	}
	// CONST 
	for( i=0; i<(uint16_t)(f_timeConst*1000); i++ ){				// [msec]
	
		f_time	 = 0.001f * (float)i;							// time[s]
		f_ang	 = f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_maxAngleV * f_time;			// angle[rad] (theta[rad] = omega[rad/s] * t[s] )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;		// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;		// Yposition[m]
	}
	//jerk DEC dec
	for( i=0; i<(uint16_t)(f_jerkAngleTime*1000); i++ ){				// [msec]
	
		f_time	 = 0.001f * (float)i;							// time[s]
		f_ang	 = f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle  + f_constAngle + 1.0 / 6.0 * f_jerkAngle*(-1.0) * f_time * f_time * f_time + f_maxAngleV * f_time;			// angle[rad] (theta[rad] = omega[rad/s] * t[s] )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;		// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;		// Yposition[m]
	}
	// DEC
	for( i=0; i<(uint16_t)(f_timeAcc*1000); i++ ){				// [msec]
	
		f_time	 = 0.001f * (float)i;							// time[s]
		f_ang	 = f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_constAngle + f_jerkDecDecAngle + 0.5f * f_angAcc * f_time * f_time + (f_maxAngleV-f_jerkAngleSpeed) * f_time;	// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;		// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;		// Yposition[m]
	}
	//jerk DEC acc
	for( i=0; i<(uint16_t)(f_jerkAngleTime*1000); i++ ){				// [msec]
	
		f_time	 = 0.001f * (float)i;							// time[s]
		f_ang	 = f_jerkAccAccAngle + f_accAngle*2.0 + f_jerkAccDecAngle + f_constAngle + f_jerkDecDecAngle + 1.0 / 6.0 * f_jerkAngle * f_time * f_time * f_time + f_jerkAngleSpeed * f_time + 1.0/2.0*(-1.0)*f_angAcc*f_time*f_time;			// angle[rad] (theta[rad] = omega[rad/s] * t[s] )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;		// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;		// Yposition[m]
	}

	// ---------------------------- 
	//  sla parameter  
	// ---------------------------- 
	p_adr->f_sla_speed				= f_speed;
	printf("enter speed %5.2f\n\r",f_speed);
	p_adr->f_sla_angJerk			= f_jerkAngle;
	printf("f_jerkAngle %5.2f\n\r",f_jerkAngle);
	p_adr->f_sla_angAcc				= f_angAcc;// * RAD_TO_DEG ;
	printf("f_angAcc %5.2f\n\r", f_angAcc);
	p_adr->f_sla_angvel				= f_maxAngleV;// * RAD_TO_DEG;
	printf("f_maxAngleV %5.2f\n\r",f_maxAngleV);
	p_adr->f_sla_ang_Total			= f_final_ang;// * RAD_TO_DEG;
	printf("f_final_ang %5.2f\n\r",f_final_ang);

	p_adr->f_sla_angS_Jerk			= f_jerkAngleSpeed;
	printf("f_jerkAngleSpeed %5.2f\n\r",f_jerkAngleSpeed);

	printf("time\n\r");
	p_adr->us_sla_jerkAngaccTime	= (uint16_t)( f_jerkAngleTime * 1000.0f );
	printf("f_jerkAngleTime %7.5f\n\r",(f_jerkAngleTime * 1000.0f));
	p_adr->us_sla_accAngvelTime		= (uint16_t)( f_timeAcc * 1000.0f );
	printf("f_timeAcc %7.5f\n\r",(f_timeAcc * 1000.0f));
	p_adr->us_sla_constAngvelTime	= (uint16_t)( f_timeConst * 1000.0f );
	printf("f_timeConst %7.5f\n\r",(f_timeConst * 1000.0f));

	printf("angle\n\r");
	p_adr->f_sla_ang_AccAccJerk	= f_jerkAccAccAngle;
	printf("f_jerkAccAccAngle %5.2f\n\r",f_jerkAccAccAngle);
	p_adr->f_sla_ang_AccConst	= f_accAngle;
	printf("f_accAngle %5.2f\n\r",f_accAngle);
	p_adr->f_sla_ang_AccDecJerk	= f_jerkAccDecAngle;
	printf("f_jerkAccDecAngle %5.2f\n\r",f_jerkAccDecAngle);
	printf("f_constAngle %5.2f\n\r",f_constAngle);
	p_adr->f_sla_ang_DecDecJerk	= f_jerkDecDecAngle;
	printf("f_jerkDecDecAngle %5.2f\n\r",f_jerkDecDecAngle);
	p_adr->f_sla_ang_DecConst	= f_accAngle;
	printf("f_accAngle(dec) %5.2f\n\r",f_accAngle);
	p_adr->f_sla_ang_DecAccJerk	= f_jerkDecAccAngle;
	printf("f_jerkDecAccAngle %5.2f\n\r",f_jerkDecAccAngle);

	printf("angleend\n\r");
	printf("f_ang_AccJerkEnd %5.2f\n\r",f_jerkAccAccAngle);
	printf("f_ang_AccConstEnd %5.2f\n\r",f_jerkAccAccAngle + f_accAngle);
	p_adr->f_sla_ang_AccEnd	= f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle;
	printf("f_ang_AccEnd %5.2f\n\r",f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle);
	p_adr->f_sla_ang_ConstEnd = f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_constAngle;
	printf("f_ang_ConstEnd %5.2f\n\r",f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_constAngle);
	printf("f_ang_DecJerkEnd %5.2f\n\r",f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_constAngle + f_jerkDecDecAngle);
	printf("f_ang_DecConstEnd %5.2f\n\r",f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_constAngle + f_jerkDecDecAngle + f_accAngle);
	printf("total %5.2f\n\r",f_jerkDecDecAngle+f_accAngle+f_jerkDecAccAngle);

	
	// calaculate enter and exit length 
	switch(en_mode){
		case SLA_90:
			p_adr->f_sla_escapeLen = f_final_x - f_x ;
			p_adr->f_sla_entryLen  = f_final_y - f_y ;
			break;

		case SLA_45:
			p_adr->f_sla_escapeLen = 1.4142f * ( f_final_x - f_x );
			p_adr->f_sla_entryLen  = f_final_y - f_y - ( f_final_x - f_x );
			break;

		case SLA_N90:
			p_adr->f_sla_escapeLen = f_final_x - f_x;
			p_adr->f_sla_entryLen  = f_final_y - f_y;
			break;

		case SLA_135:
			p_adr->f_sla_escapeLen = 1.4142f * ( f_final_x - f_x );
			p_adr->f_sla_entryLen  = f_final_y - f_y + ( f_final_x - f_x );
			break;

		default:
			break;
	}
	printf("entry %6.4f\n\r",f_final_x - f_x);
	printf("escape %6.4f\n\r",f_final_y - f_y);
}

stSLA* PARAM_getSra( enSLA_TYPE en_mode )
{
	return &st_Sla[en_mode];
}
