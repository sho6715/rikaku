/*
 * encoder.c
 *
 *  Created on: Jan 13, 2021
 *      Author: shohe
 */


#include "hal/encoder.h"

void ENC_clr( void )
{
	Clear_CounterTIM5();
	Clear_CounterTIM1();
}

uint16_t Get_encoder_value(en_ENDIR dir)
{
	if(dir == EN_L) return Get_CounterTIM1();
	else return Get_CounterTIM5();
}

void ENC_GetDiv( int32_t* p_r, int32_t* p_l )
{
	int64_t l_cntR = (int64_t) Get_CounterTIM5();
	int64_t l_cntL = (int64_t) Get_CounterTIM1();

	ENC_clr();

	*p_r = ENC_RESET_VAL - l_cntR;		// 右モータ
	*p_l = l_cntL - ENC_RESET_VAL;		// 左モータ
}

void ENC_print(void)
{
//	printf("エンコーダ [R]=%d [L]=%d \r",ENC_R_CNT*360/65536,ENC_L_CNT*360/65536);
//	printf("%d,%d \r\n",ENC_R_CNT*360/65536,ENC_L_CNT*360/65536);
}

