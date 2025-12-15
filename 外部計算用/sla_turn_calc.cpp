
#include "stdio.h"
#include "math.h"

#define HALF_BLOCK			( 0.045f )					
#define BLOCK				( 0.09f )					
#define HALF_BLOCK_SKEW			( 0.06364f )
#define BLOCK_SKEW			( 0.12728f )	
#define DEG_TO_RAD  (3.1416f/180.0f)
#define RAD_TO_DEG  (180.0f/3.1416f)

#define PI							( 3.14159f )		

typedef struct{
	float	f_sla_speed;
	float	f_sla_angJerk;
	float	f_sla_angAcc;
	float	f_sla_angvel;
	float	f_sla_entryLen;
	float	f_sla_escapeLen;
	int	us_sla_jerkAngaccTime;
	int	us_sla_accAngvelTime;
	int	us_sla_constAngvelTime;
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

stSLA				st_Sla[SLA_TYPE_MAX];					// スラローム時の走行パラメータ

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
	int	i = 0;						// ループ用
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
	for( i=0; i<(int)(f_jerkAngleTime*1000); i++ ){
		f_time	=  0.001f * (float)i;								// time[s]
//		f_ang	=  0.5f * f_angAcc * f_time * f_time;				// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_ang	= 1.0 / 6.0 * f_jerkAngle * f_time * f_time * f_time;
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;			// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;			// Yposition[m]
	}
	// ACC 
	for( i=0; i<(int)(f_timeAcc*1000); i++ ){				// [msec]
		f_time	=  0.001f * (float)i;								// time[s]
		f_ang	= f_jerkAccAccAngle + 0.5f * f_angAcc * f_time * f_time+ f_jerkAngleSpeed*f_time;				// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;			// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;			// Yposition[m]
	}
	//jerk ACC dec
	for( i=0; i<(int)(f_jerkAngleTime*1000); i++ ){
		f_time	=  0.001f * (float)i;								// time[s]
//		f_ang	=  0.5f * f_angAcc * f_time * f_time;				// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_ang	= f_jerkAccAccAngle + f_accAngle + 1.0 / 6.0 * f_jerkAngle * (-1.0) * f_time * f_time * f_time + (f_maxAngleV-f_jerkAngleSpeed) * f_time + 1.0/2.0*f_angAcc*f_time*f_time;
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;			// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;			// Yposition[m]
	}
	// CONST 
	for( i=0; i<(int)(f_timeConst*1000); i++ ){				// [msec]
		f_time	 = 0.001f * (float)i;							// time[s]
		f_ang	 = f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_maxAngleV * f_time;			// angle[rad] (theta[rad] = omega[rad/s] * t[s] )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;		// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;		// Yposition[m]
	}
	//jerk DEC dec
	for( i=0; i<(int)(f_jerkAngleTime*1000); i++ ){				// [msec]
		f_time	 = 0.001f * (float)i;							// time[s]
		f_ang	 = f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle  + f_constAngle + 1.0 / 6.0 * f_jerkAngle*(-1.0) * f_time * f_time * f_time + f_maxAngleV * f_time;			// angle[rad] (theta[rad] = omega[rad/s] * t[s] )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;		// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;		// Yposition[m]
	}
	// DEC
	for( i=0; i<(int)(f_timeAcc*1000); i++ ){				// [msec]
		f_time	 = 0.001f * (float)i;							// time[s]
		f_ang	 = f_jerkAccAccAngle + f_accAngle + f_jerkAccDecAngle + f_constAngle + f_jerkDecDecAngle + 0.5f * f_angAcc * f_time * f_time + (f_maxAngleV-f_jerkAngleSpeed) * f_time;	// angle[rad] (theta[rad] = 1/2 * a[rad/s^2] * t[s]^2 )
		f_x		+= f_speed * (float)sin( f_ang ) * 0.001f;		// Xposition[m]
		f_y		+= f_speed * (float)cos( f_ang ) * 0.001f;		// Yposition[m]
	}
	//jerk DEC acc
	for( i=0; i<(int)(f_jerkAngleTime*1000); i++ ){				// [msec]
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
	p_adr->us_sla_jerkAngaccTime	= (int)( f_jerkAngleTime * 1000.0f );
	printf("f_jerkAngleTime %7.5f\n\r",(f_jerkAngleTime * 1000.0f));
	p_adr->us_sla_accAngvelTime		= (int)( f_timeAcc * 1000.0f );
	printf("f_timeAcc %7.5f\n\r",(f_timeAcc * 1000.0f));
	p_adr->us_sla_constAngvelTime	= (int)( f_timeConst * 1000.0f );
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


int main(void){
	printf("turn calc\r\n");
    printf("\r\n turn 45 \r\n");
	PARAM_makeSra( 0.5, 500.0f, 7.00f,6000*PI,  SLA_45 );
	printf("\r\n turn 90 \r\n");		
	PARAM_makeSra( 0.5, 600.0f, 10.00f,6000*PI,  SLA_90 );	
	printf("\r\n turn 135 \r\n");	
	PARAM_makeSra( 0.5, 650.0f, 12.00f,6000*PI,  SLA_135 );	
	printf("\r\n turn N90 \r\n");	
	PARAM_makeSra( 0.5, 850.0f, 14.00f,12000*PI,  SLA_N90 );	

/*
	printf("\r\n turn 45 \r\n");
	PARAM_makeSra( 1.0, 1300.0f, 25.00f,35000*PI,  SLA_45 );
	printf("\r\n turn 90 \r\n");		
	PARAM_makeSra( 1.0, 1600.0f, 33.00f,55000*PI,  SLA_90 );	
	printf("\r\n turn 135 \r\n");	
	PARAM_makeSra( 1.0, 1500.0f, 40.00f,55000*PI,  SLA_135 );	
	printf("\r\n turn N90 \r\n");	
	PARAM_makeSra( 1.0, 2100.0f, 45.00f,180000*PI,  SLA_N90 );	
*/
/*
	printf("\r\n turn 45 \r\n");
	PARAM_makeSra( 1.5, 3000.0f, 50.00f,100000*PI,  SLA_45 );
	printf("\r\n turn 90 \r\n");		
	PARAM_makeSra( 1.5, 3900.0f, 70.00f,200000*PI,  SLA_90 );	
	printf("\r\n turn 135 \r\n");	
	PARAM_makeSra( 1.5, 3500.0f, 90.00f,300000*PI,  SLA_135 );	
	printf("\r\n turn N90 \r\n");	
	PARAM_makeSra( 1.5, 4200.0f, 110.00f,450000*PI,  SLA_N90 );	
*/
//	while(1);
}