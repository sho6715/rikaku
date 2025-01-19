/*
 * parameter.h
 *
 *  Created on: 2021/02/04
 *      Author: shohe
 */

#ifndef INC_PARAMETER_H_
#define INC_PARAMETER_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#else
#include "glob.h"
#endif

#define HALF_BLOCK			( 0.045f )					
#define BLOCK				( 0.09f )					
#define HALF_BLOCK_SKEW			( 0.06364f )
#define BLOCK_SKEW			( 0.12728f )		

#define		SW_CHATTERING_WAIT		(200) 	

#define	FF_BALANCE_R				( 1.00f )					
#define	FF_BALANCE_L				( 1.00f )		
#define FF_HIT_BALANCE_R			(1.00f)				
#define FF_HIT_BALANCE_L			(1.00f)

#define SEARCH_SPEED				(0.3)
#define MAP_KNOWN_ACC_SPEED			(0.5)

#define MOT_WALL_EDGE_DIST			( 0.0305f )//28
/*
#define 	f_FB_speed_kp		(35.0)//35
#define 	f_FB_speed_ki		(2.0)//4
#define 	f_FB_speed_kd		(0.1)//0.3
#define		f_FB_angleS_kp		(95.0)//95
#define		f_FB_angleS_ki		(1.0)//6.0
#define		f_FB_angleS_kd		(30.0)//0.2

#define		f_FB_angle_kp		(0.0)//0.45
#define		f_FB_angle_ki		(0.0)//0.25

#define		f_FB_wall_kp		(0.4)//0.4
#define		f_FB_wall_kd		(0.2)//0.2
*/
#define		FB_FRONT_WALL_V_KP	(0.001)
#define		FB_FRONT_WALL_V_KI	(0.0)
#define		FB_FRONT_WALL_V_KD	(0.0001)
#define		FB_FRONT_WALL_OMEGA_KP	(0.9)
#define		FB_FRONT_WALL_OMEGA_KI	(0.0)
#define		FB_FRONT_WALL_OMEGA_KD	(0.3)

typedef enum{
	
	PARAM_ST_TOP = 0,		

		PARAM_ACC,			
		PARAM_CONST,			
		PARAM_DEC,		
//		PARAM_BACK_ACC,			
//		PARAM_BACK_CONST,		
//		PARAM_BACK_DEC,			
		PARAM_SKEW_ACC,		
		PARAM_SKEW_CONST,	
		PARAM_SKEW_DEC,		
		PARAM_HIT_WALL,		

	PARAM_ST_BTM,		

	PARAM_TRUN_TOP,	

		PARAM_ACC_TRUN,		
		PARAM_CONST_TRUN,	
		PARAM_DEC_TRUN,	
	
	PARAM_TRUN_BTM,	

	PARAM_SLA_TOP,			

		PARAM_ENTRY_SLA,	
		PARAM_ACC_SLA,		
		PARAM_CONST_SLA,	
		PARAM_DEC_SLA,	
		PARAM_EXIT_SLA,		

	PARAM_SLA_BTM,			
	
	PARAM_ST,		
	PARAM_TRUN,			
	PARAM_SLA,	

	PARAM_ST_MAX		= PARAM_ST_BTM   - PARAM_ST_TOP,		
	PARAM_TRUN_MAX		= PARAM_TRUN_BTM - PARAM_TRUN_TOP,		
	PARAM_SLA_MAX		= PARAM_SLA_BTM  - PARAM_SLA_TOP,			
	
	PARAM_NC = 0xff,
	
}enPARAM_MODE;


typedef enum{
	
	PARAM_VERY_SLOW = 0,	
	PARAM_SLOW,				
	PARAM_NORMAL,			
	PARAM_FAST,				
	PARAM_VERY_FAST,		

	PARAM_MOVE_SPEED_MAX
	
}enPARAM_MOVE_SPEED;

typedef struct{
	float			f_speed_jerk;
	float			f_speed_acc;					
	float			f_speed_dec;
	float			f_speed_jerkangle;					
	float			f_speed_accangle;				
	float			f_speed_decangle;				
}stSPEED;

/* ゲイン */
typedef struct{
	float 			f_FB_speed_kp;			// フィードバック、速度 比例制御
	float 			f_FB_speed_ki;			// フィードバック、速度 積分制御
	float 			f_FB_speed_kd;			// フィードバック、速度 微分制御
	float			f_FB_angleS_kp;			// フィードバック、角速度 比例制御
	float			f_FB_angleS_ki;			// フィードバック、角速度 積分制御
	float			f_FB_angleS_kd;			// フィードバック、角速度 微分制御
	float			f_FB_angle_kp;			// フィードバック、角度 比例制御
	float			f_FB_angle_ki;			// フィードバック、角度 積分制御
	float			f_FB_wall_kp;			// フィードバック、壁 比例制御
	float			f_FB_wall_kd;			// フィードバック、壁 微分制御
}stGAIN;

typedef struct{
	float	f_sla_speed;
	float	f_sla_angJerk;
	float	f_sla_angAcc;
	float	f_sla_angvel;
	float	f_sla_entryLen;
	float	f_sla_escapeLen;
	uint16_t	us_sla_jerkAngaccTime;
	uint16_t	us_sla_accAngvelTime;
	uint16_t	us_sla_constAngvelTime;
	float	f_sla_angS_Jerk;
	float	f_sla_ang_AccAccJerk;
	float	f_sla_ang_AccConst;
	float	f_sla_ang_AccDecJerk;
	float	f_sla_ang_DecDecJerk;
	float	f_sla_ang_DecConst;
	float	f_sla_ang_DecAccJerk;
	float	f_sla_ang_AccEnd;
	float	f_sla_ang_ConstEnd;
	float	f_sla_ang_Total;
}stSLA;


typedef enum{
	SLA_90,
	SLA_45,	
	SLA_135,
	SLA_N90,				
	SLA_TYPE_MAX
}enSLA_TYPE;


void PARAM_setSpeedType( enPARAM_MODE en_mode, enPARAM_MOVE_SPEED en_speed );
const stSPEED* PARAM_getSpeed( enPARAM_MODE en_mode );
const stGAIN* PARAM_getGain( enPARAM_MODE en_mode );
enPARAM_MOVE_SPEED PARAM_getSpeedType( enPARAM_MODE en_mode );

void PARAM_makeSra( float f_speed, float f_angAcc, float f_g , float f_jerkAngle, enSLA_TYPE en_mode);
stSLA* PARAM_getSra( enSLA_TYPE en_mode );
#endif /* INC_PARAMETER_H_ */
