/*
 * sen_batt.c
 *
 *  Created on: 2021/01/13
 *      Author: shohe
 */

#include "hal/sen_batt.h"

#define		BAT_GOOD			(3060)			//
#define		BAT_LOW				(2730)			//

uint16_t	us_BatLvAve = 4095;							// バッテリ平均値（AD変換の最大値で初期化）


void BAT_Pol( void )
{
	static uint16_t 	us_batLv[5] = { 4095, 4095, 4095, 4095, 4095 };
	static uint8_t		i = 0;

	us_batLv[4] = us_batLv[3];
	us_batLv[3] = us_batLv[2];
	us_batLv[2] = us_batLv[1];
	us_batLv[1] = us_batLv[0];

	us_batLv[0] = GetBatVal();

	us_BatLvAve = ( us_batLv[0] + us_batLv[1] + us_batLv[2] + us_batLv[3] + us_batLv[4] ) / 5;


	if( us_BatLvAve < BAT_LOW ) {
		SetBatLED(0);
	}
	else if( us_BatLvAve < BAT_GOOD ) {
		if( i>=100){
			SetBatLED(1);
			i=0;
		}
		else if(i>=50){
			SetBatLED(0);
		}
		i++;
	}
	else{
		SetBatLED(1);
	}
}

float get_battLv(void){
	return (float)us_BatLvAve*(20.0+10.0)/10.0/4095.000*3.300;
}
