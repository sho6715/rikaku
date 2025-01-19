/*
 * log.c
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#include "hal/log.h"


#define log_num			(2000)					//ログ取得数（変更時はこちらを変更）
//ログプログラム群（取得数変更はdefineへ）
/*short	Log_1[log_num];
short	Log_2[log_num];
short	Log_3[log_num];
short	Log_4[log_num];
short	Log_5[log_num];
short	Log_6[log_num];
short	Log_7[log_num];
short	Log_8[log_num];
short	Log_9[log_num];
short	Log_10[log_num];
short	Log_11[log_num];
short	Log_12[log_num];
short	Log_13[log_num];
short	Log_14[log_num];
*/
float	Log_1[log_num];
float	Log_2[log_num];
float	Log_3[log_num];
float	Log_4[log_num];
float	Log_5[log_num];
float	Log_6[log_num];
float	Log_7[log_num];
float	Log_8[log_num];
float	Log_9[log_num];
float	Log_10[log_num];
float	Log_11[log_num];
float	Log_12[log_num];

uint16_t	log_count = 0;
bool	b_logflag = FALSE;

float TempLog1,TempLog2,TempLog3,TempLog4,Duty_L,Duty_R ; 

/*
void log_in2( 	short log1,short log2,
			short log3,short log4,
			short log5,short log6,
			short log7,short log8,
			short log9,short log10,
			short log11,short log12,
			short log13,short log14)
*/
void log_in2( 	float log1,float log2,
			float log3,float log4,
			float log5,float log6,
			float log7,float log8,
			float log9,float log10,
			float log11,float log12)
{
	if((b_logflag == TRUE)&&(log_count < log_num)){
		Log_1[log_count] = log1;
		Log_2[log_count] = log2;
		Log_3[log_count] = log3;
		Log_4[log_count] = log4;
		Log_5[log_count] = log5;
		Log_6[log_count] = log6;
		Log_7[log_count] = log7;
		Log_8[log_count] = log8;
		Log_9[log_count] = log9;
		Log_10[log_count] = log10;
		Log_11[log_count] = log11;
		Log_12[log_count] = log12;
//		Log_13[log_count] = log13;
//		Log_14[log_count] = log14;

		log_count++;
	}
}

void log_interrupt ( void )
{
	log_in2(GYRO_getSpeedErr(), Get_TrgtAngleS(),
			Get_NowAngle(),Get_TrgtAngle(),
			Get_NowSpeed(), Get_TrgtSpeed(),
			Get_NowDist(), Get_TrgtDist(),TempLog1,TempLog2,TempLog3,TempLog4);

}

void log_flag_on(void)
{
	b_logflag = TRUE;
}

void log_flag_off(void)
{
	b_logflag = FALSE;
}

void log_read2(void)
{
	int16_t i=0;
	while(i<log_num){
	/*	printf("%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f\n\r",
		(float)Log_1[i]/1000.0,(float)Log_2[i]/1000.0,(float)Log_3[i]/1000.0,(float)Log_4[i]/1000.0,(float)Log_5[i]/1000.0,
		(float)Log_6[i]/1000.0,(float)Log_7[i]/1000.0,(float)Log_8[i]/1000.0,(float)Log_9[i]/1000.0,(float)Log_10[i]/1000.0,
		(float)Log_11[i]/1000.0,(float)Log_12[i]/1000.0,(float)Log_13[i]/1000.0,(float)Log_14[i]/1000.0);
*/
		printf("%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f\n\r",
		Log_1[i],Log_2[i],Log_3[i],Log_4[i],Log_5[i],Log_6[i],Log_7[i],Log_8[i],Log_9[i],Log_10[i],Log_11[i],Log_12[i]);
		
		i++;
	}

/*
	while(i<log_num){
		printf("%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f,%5.4f\n\r",
		Log_1[i],Log_2[i],Log_3[i],Log_4[i],Log_5[i],Log_6[i],Log_7[i],Log_8[i],Log_9[i],Log_10[i]);
		i++;
	}
*/
}
