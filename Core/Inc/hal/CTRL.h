/*
 * CTRL.h
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#ifndef INC_CTRL_H_
#define INC_CTRL_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#include "Inc/hal/DCM.h"
#include "Inc/hal/gyro.h"
#include "Inc/gpio.h"
#include "Inc/hal/run.h"
#include "Inc/parameter.h"
#include "Inc/hal/encoder.h"
#else
#include "glob.h"
#include "hal/DCM.h"
#include "hal/gyro.h"
#include "gpio.h"
#include "hal/run.h"
#include "parameter.h"
#include "hal/encoder.h"
#endif

/* motion type */
typedef enum{
	CTRL_ACC,				// [00] 
	CTRL_CONST,				// [01] 
	CTRL_DEC,				// [02] 

	CTRL_SKEW_ACC,			// [03] 
	CTRL_SKEW_CONST,		// [04] 
	CTRL_SKEW_DEC,			// [05] 
	
	CTRL_HIT_WALL,			// [06]
	
	CTRL_ACC_TRUN,			// [07] 
	CTRL_CONST_TRUN,		// [08] 
	CTRL_DEC_TRUN,			// [09] 
	
	CTRL_ENTRY_SLA,		// [10]
	CTRL_ACC_SLA,			// [11] 
	CTRL_CONST_SLA,		// [12] 
	CTRL_DEC_SLA,			// [13] 
	CTRL_EXIT_SLA,			// [14] 

	CTRL_FRONT_WALL,		// [15]

	CTRL_MAX,

}enCTRL_TYPE;

/* 制御データ */
typedef struct{
	enCTRL_TYPE		en_ctrl_type;		// motion type
	float			f_ctrl_time;			// target time[sec]
	float			f_ctrl_jerk;			// jerk[m/s3]
	float			f_ctrl_nowAcc;			// accel speed[m/s2]
	float			f_ctrl_trgtAcc;			// max accel speed[m/s2]
	float			f_ctrl_now;			// [speed ctrl] now speed[m/s]
	float			f_ctrl_trgt;			// [speed ctrl] finish speed[m/s]
	float			f_ctrl_nowDist;		// [dist ctrl] now distance[m]
	float			f_ctrl_dist;			// [dist ctrl] finish distance[m]
	float			f_ctrl_jerkAngle;	// jerk angle[rad/s3]
	float			f_ctrl_nowAccAngle;
	float			f_ctrl_trgtAccAngle;	// [angle speed ctrl] angle accel speed[rad/s2]
	float			f_ctrl_nowAngleS;	// [angle speed ctrl] now angle speed[rad/s]
	float			f_ctrl_trgtAngleS;	// [angle speed ctrl] finish angle speed[rad/s]
	float			f_ctrl_nowAngle;		// [angle ctrl] now angle[rad]
	float			f_ctrl_angle;		// [angle ctrl] finish angle[rad]
}stCTRL_DATA;

extern enCTRL_TYPE		en_Type;						// motion type
//escape time 
extern float	EscapeWait;

extern uint8_t	uc_DistControl;	//スラロームの前距離を伸ばして処理している箇所　意外と効いてる可能性あり

float Get_NowSpeed(void);
float Get_NowDist(void);
float Get_TrgtDist(void);
float Get_TrgtSpeed(void);
void Set_TrgtSpeed(float speed);
float Get_NowDistR(void);
float Get_NowAngle(void);
float Get_TrgtAngle(void);
float Get_TrgtAngleS(void);
void INTC_sys(void);
void CTRL_sta( void );
void CTRL_stop( void );
void CTRL_pol( void );
void CTRL_clrData( void );
void CTRL_clrNowData( void );
void CTRL_clrTrgtIntegra(void);
void CTRL_clrAngleErrSum(void);
void CTRL_clrSpeedErrSum(void);
void CTRL_setNowData_Err(/*float trgtDist, */float trgtAngle);
void CTRL_setData( stCTRL_DATA* p_data );
void CTRL_refNow( void );
void CTRL_refTarget( void );
void CTRL_outMot( float f_duty10_R, float f_duty10_L );
void Failsafe_flag(void);
void Failsafe_flag_off(void);
bool SYS_isOutOfCtrl( void );

#endif /* INC_CTRL_H_ */
