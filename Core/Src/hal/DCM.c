/*
 * DCM.c
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#include "hal/DCM.h"

void DCM_setDirCw( enDCM_ID en_id )
{
	/* 回転方向設定 */
	if( en_id == DCM_R ){			// 右
		Set_MOT0(1);	//tmp
	}
	else{							// 左
		Set_MOT1(0);	//tmp

	}
}

void DCM_setDirCcw( enDCM_ID en_id )
{
	/* 回転方向設定 */
	if( en_id == DCM_R ){			// 右
		Set_MOT0(0);	//tmp
	}
	else{							// 左
		Set_MOT1(1);	//tmp
	}
}

void DCM_brakeMot( enDCM_ID en_id )
{
	/* 停止設定 */
	if( en_id == DCM_R ){			// 右
		Set_DutyTIM2(0);
	}
	else{							// 左
		Set_DutyTIM3(0);
	}
}

void DCM_staMot( enDCM_ID en_id )
{	
	/* タイマスタート */
	if( en_id == DCM_R ){			// 右
		Enable_TIM2();
	}
	else{							// 左
	   Enable_TIM3();
	}
}

void DCM_staMotAll( void )
{
	DCM_staMot(DCM_R);									// 右モータON
	DCM_staMot(DCM_L);									// 左モータON
}

void DCM_setPwmDuty( enDCM_ID en_id, uint16_t us_duty10 )
{
	/* PWM設定 */
	if( en_id == DCM_R ){				// 右

		if( 0 == us_duty10 ){			// Duty0%設定
			DCM_brakeMot( en_id );
		}
		else{
			Set_DutyTIM2(us_duty10);
			DCM_staMot( en_id );		// 回転開始
		}
	}
	else{								// 左

		if( 0 == us_duty10 ){			// Duty0%
			DCM_brakeMot( en_id );
		}
		else{
			Set_DutyTIM3(us_duty10);
			DCM_staMot( en_id );		// 回転開始
		}
	}
}




