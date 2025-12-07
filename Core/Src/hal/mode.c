/*
 * mode.c
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */


#include "hal/mode.h"

#define MODELED_1  (0x01)
#define MODELED_2  (0x10)

typedef enum{
	MODE_0 = 0,
	MODE_1,
	MODE_2,
	MODE_3,
	MODE_4,
	MODE_5,
	MODE_6,
	MODE_7,
	MODE_MAX
}enMODE;

uint8_t NowModeLed = MODELED_1;

enMODE		en_Mode;		//現在のモー�?

uint8_t		WallHitFlag = 0; //0 = not wall_hit

extern uint8_t			SLA_Count;

void SYS_start( void )
{
	/* タイトル表示 */
	printf(" ------------------------------\r\n");
	printf(" | Robo Name  : hankyo2       |\r\n");
	printf(" | Developer  : sho sato      |\r\n");
	printf(" | Version    : ver1          |\r\n");
	printf(" | Project By : RT Corporation|\r\n");
	printf(" ------------------------------\r\n");	

	printf("\r\n turn 45 \r\r");
	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 2.00f,3000*PI, SLA_45 );
	printf("\r\n turn 90 \r\r");		
	PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 3.50f,3000*PI,  SLA_90 );	
//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f, SLA_90 );
	printf("\r\n turn 135 \r\r");	
	PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 4.00f,3000*PI,  SLA_135 );	
	printf("\r\n turn N90 \r\r");	
	PARAM_makeSra( (float)SEARCH_SPEED, 300.0f, 5.00f,3500*PI,  SLA_N90 );		


/*
	printf("\r\n turn 45 \r\r");
	PARAM_makeSra( 0.5, 550.0f, 7.50f,3000*PI,  SLA_45 );
	printf("\r\n turn 90 \r\r");		
	PARAM_makeSra( 0.5, 700.0f, 10.00f,3000*PI,  SLA_90 );	
//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
	printf("\r\n turn 135 \r\r");	
	PARAM_makeSra( 0.5, 750.0f, 10.00f,3000*PI,  SLA_135 );	
	printf("\r\n turn N90 \r\r");	
	PARAM_makeSra( 0.5, 800.0f, 11.00f,3000*PI,  SLA_N90 );	
*/
}

void MODE_inc( void )
{
	en_Mode++;		// モードを�?�?

	/* 最大値チェ�?ク */
	if( MODE_MAX == en_Mode ){
		en_Mode = MODE_0;
	}

	/* モード表示 */
	switch( en_Mode ){

		case MODE_0:
			SetLED(0x00 | NowModeLed);
			break;

		case MODE_1:
			SetLED((0x01<<1) | NowModeLed);
			break;

		case MODE_2:
			SetLED((0x02<<1) | NowModeLed);
			break;

		case MODE_3:
			SetLED((0x03<<1) | NowModeLed);
			break;

		case MODE_4:
			SetLED((0x04<<1) | NowModeLed);
			break;

		case MODE_5:
			SetLED((0x05<<1) | NowModeLed);
			break;

		case MODE_6:
			SetLED((0x06<<1) | NowModeLed);
			break;

		case MODE_7:
			SetLED((0x07<<1) | NowModeLed);
			break;

		default:
			break;
	}
}

bool CountUP_mode(void){
	if(Get_NowDistR()>0.01){
    CTRL_clrNowData();
    return TRUE;
  }else{
    return FALSE;
  }
}

void MODE_speed_parameter(
	enPARAM_MOVE_SPEED	speed_para,
	enPARAM_MOVE_SPEED	turn_para,
	enPARAM_MOVE_SPEED	sla_para,
	float				trgt_speed,
	float				sla_sp_90,
	float				sla_sp_45,
	float				sla_sp_135,
	float				sla_sp_N90
){
	PARAM_setSpeedType( PARAM_ST,   speed_para );							// [直進] 速度普�?
	PARAM_setSpeedType( PARAM_TRUN, turn_para );							// [旋回] 速度普�?
	PARAM_setSpeedType( PARAM_SLA,  sla_para );							// [スラ] 速度普�?
	MOT_setTrgtSpeed(trgt_speed);
	MOT_setSlaStaSpeed( sla_sp_90, SLA_90);							// スラロー�?開始速度設�?
	MOT_setSlaStaSpeed( sla_sp_45, SLA_45);
	MOT_setSlaStaSpeed( sla_sp_135, SLA_135);
	MOT_setSlaStaSpeed( sla_sp_N90, SLA_N90);
		
}

void MODE_exe_m0( void )
{
	enMAP_HEAD_DIR		en_endDir2;
	GYRO_SetRef();

	CTRL_clrData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
	CTRL_clrNowData();
	/* モード表示 */
	switch( en_Mode ){

		case MODE_0:
			SetLED(0x0e);
			CTRL_clrNowData();
			CTRL_clrData();
//			LL_TIM_EnableIT_UPDATE(TIM4);
//			LL_TIM_EnableCounter(TIM4);
			printf("\n");
			LL_mDelay(1000);
			GYRO_SetRef();
			while(1){
				printf("gyro%5.2f ref%5.2f ",
					GYRO_getNowAngle(),GYRO_getRef()
				);
				printf("err %5.2f ",GYRO_getSpeedErr());
				printf("s_val%x\r",Get_s_gyro());
				LL_mDelay( 500 );
			}
			break;

		case MODE_1:
			SetLED(0x0e);
			printf("\n");
			LL_mDelay(1000);
			SetLED(0x00);
			while(1){
				printf("  ENC_R%5d ENC_L%5d \r", 
					Get_encoder_value(EN_R),Get_encoder_value(EN_L)
				);
				LL_mDelay( 500 );
			}
			break;

		case MODE_2:
			SetLED(0x0e);
			log_read2();
			break;

		case MODE_3:
			SetLED(0x0e);
			log_flag_on();
/*
			DCM_setDirCcw( DCM_R );
			DCM_setPwmDuty( DCM_R, 280 );
			DCM_setDirCw( DCM_L );
			DCM_setPwmDuty( DCM_L, 280 );

			LL_mDelay(2000);
			DCM_setPwmDuty( DCM_R, 0 );
			DCM_setPwmDuty( DCM_L, 0 );
*/
			
			Set_DutyTIM8(600);
			LL_mDelay(10000);
			log_flag_off();
			SetLED(0x0e);
			break;

		case MODE_4:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							// [直進] 速度普�?
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							// [旋回] 速度普�?
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );							// [スラ] 速度普�?
*/
//			printf("mode %d\n\r",MOT_getStType( 2.0, 0.0, MOT_GO_ST_NORMAL));
//			MOT_getStType( 2.0, 0.0, MOT_GO_ST_NORMAL );

			SetLED(0x00);
			LL_mDelay(500);
			CTRL_clrData();
			CTRL_clrAngleErrSum();
			CTRL_clrSpeedErrSum();
			CTRL_clrNowData();
			log_flag_on();
			LL_mDelay(10);

			MOT_goBlock_FinSpeed(0.5, SEARCH_SPEED);
//			MOT_goBlock_Const(1);
			MOT_goSla(MOT_R90S, PARAM_getSra( SLA_90 ));
			MOT_goBlock_FinSpeed(0.5, 0);

/*
			MOT_goBlock_FinSpeed(0.5,SEARCH_SPEED);
			MOT_goSla(MOT_L45S_S2N,PARAM_getSra( SLA_45 ));
			MOT_goSkewBlock_FinSpeed(0.5,SEARCH_SPEED);
			MOT_goSla(MOT_R135S_N2S,PARAM_getSra( SLA_135 ));
			MOT_goBlock_FinSpeed(0.5, 0);
*/
/*
			MOT_turn(MOT_R90);
			LL_mDelay(500);
			MOT_turn(MOT_L90);
			LL_mDelay(500);
*/
/*
//			MOT_setTrgtSpeed(SEARCH_SPEED/4.0);		// 目標速度
			MOT_goBlock_FinSpeed(2.0,0);
			MOT_setTrgtSpeed(SEARCH_SPEED);		// 目標速度
*/
			log_flag_off();
			break;

		case MODE_5:
			SetLED(0x0e);

			printf("\r\n turn 45 \r\r");
			PARAM_makeSra( 0.5, 550.0f, 7.50f,3000*PI,  SLA_45 );
			printf("\r\n turn 90 \r\r");		
			PARAM_makeSra( 0.5, 700.0f, 10.00f,3000*PI,  SLA_90 );	
		//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
			printf("\r\n turn 135 \r\r");	
			PARAM_makeSra( 0.5, 750.0f, 10.00f,3000*PI,  SLA_135 );	
			printf("\r\n turn N90 \r\r");	
			PARAM_makeSra( 0.5, 800.0f, 11.00f,3000*PI,  SLA_N90 );	
			MODE_speed_parameter(PARAM_VERY_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*4.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*4.0);
			MOT_setSlaStaSpeed( 0.5, SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5, SLA_45);
			MOT_setSlaStaSpeed( 0.5, SLA_135);
			MOT_setSlaStaSpeed( 0.5, SLA_N90);
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							// [直進] 速度普�?
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							// [旋回] 速度普�?
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							// [スラ] 速度普�?
*/
			SetLED(0x00);
			LL_mDelay(500);
			CTRL_clrData();
			CTRL_clrAngleErrSum();
			CTRL_clrSpeedErrSum();
			CTRL_clrNowData();
			Set_DutyTIM8(600);
			LL_mDelay(2000);
			log_flag_on();
/*
			MOT_goBlock_FinSpeed(0.5, 0.5);
			MOT_goSla(MOT_R90S, PARAM_getSra( SLA_90 ));
			MOT_goBlock_FinSpeed(0.5, 0);
*/
/*
			MOT_goBlock_FinSpeed(1.0, 0.5);
			MOT_goSla(MOT_R45S_S2N, PARAM_getSra( SLA_45 ));
			MOT_goSkewBlock_FinSpeed(0.5, 0);
*/
/*
			MOT_goSkewBlock_FinSpeed(0.5, 0.5);
			MOT_goSla(MOT_R90S_N, PARAM_getSra( SLA_N90 ));
			MOT_goSkewBlock_FinSpeed(0.5, 0);
*/

			MOT_goBlock_FinSpeed(0.5, 0.5);
			MOT_goSla(MOT_R135S_S2N, PARAM_getSra( SLA_135 ));
			MOT_goSkewBlock_FinSpeed(0.5, 0);

			log_flag_off();

			Set_DutyTIM8(0);
			LL_mDelay(2000);

			PARAM_makeSra( (float)SEARCH_SPEED, 100.0f, 2.50f,3000*PI,  SLA_45 );
			printf("\r\n turn 90 \r\r");		
			PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 3.50f,3000*PI,  SLA_90 );	
		//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
			printf("\r\n turn 135 \r\r");	
			PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 4.00f,3000*PI,  SLA_135 );	
			printf("\r\n turn N90 \r\r");	
			PARAM_makeSra( (float)SEARCH_SPEED, 300.0f, 4.00f,3000*PI,  SLA_N90 );		
			MODE_speed_parameter(PARAM_VERY_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);
*/
			break;

		case MODE_6:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							// [直進] 速度普�?
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							// [旋回] 速度普�?
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );							// [スラ] 速度普�?
*/
			SetLED(0x00);
			LL_mDelay(500);
			CTRL_clrData();
			CTRL_clrAngleErrSum();
			CTRL_clrSpeedErrSum();
			CTRL_clrNowData();
			log_flag_on();

			MOT_setTrgtSpeed( SEARCH_SPEED );
			MOT_goBlock_FinSpeed(15.0, 0);

//			MOT_turn(MOT_R90);
/*			
			MOT_goBlock_FinSpeed(0.5, SEARCH_SPEED);
			MOT_goSla(MOT_R90S, PARAM_getSra( SLA_90 ));
//			MOT_goSla(MOT_L90S, PARAM_getSra( SLA_90 ));
			MOT_goBlock_FinSpeed(0.5, 0);
*/
/*
			MOT_goBlock_FinSpeed(1.0, SEARCH_SPEED);
			MOT_goSla(MOT_R45S_S2N, PARAM_getSra( SLA_45 ));
			MOT_goSkewBlock_FinSpeed(0.5, 0);
*/
/*
			MOT_goSkewBlock_FinSpeed(0.5, SEARCH_SPEED);
			MOT_goSla(MOT_R90S_N, PARAM_getSra( SLA_N90 ));
			MOT_goSkewBlock_FinSpeed(0.5, 0);
*/
/*
			MOT_goBlock_FinSpeed(0.5, SEARCH_SPEED);
			MOT_goSla(MOT_R135S_S2N, PARAM_getSra( SLA_135 ));
			MOT_goSkewBlock_FinSpeed(0.5, 0);
*/
			log_flag_off();
			break;

		case MODE_7:
			SetLED(0x0e);
			//cant use
			break;

		default:
			break;
	}
}

void MODE_exe_m1( void )
{
	enMAP_HEAD_DIR		en_endDir2;

	uint64_t data =0;

	GYRO_SetRef();
	CTRL_clrData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
	CTRL_clrNowData();

	switch( en_Mode ){

		case MODE_0:
			SetLED(0x0e);
			break;

		case MODE_1:
			SetLED(0x0e);
			Map_Erase();
			break;

		case MODE_2:
			SetLED(0x0e);
			MAP_showLog();
			break;

		case MODE_3:
			SetLED(0x0e);
			Map_Copy();
			break;

		case MODE_4:
			SetLED(0x0e);
			Map_Write();
			break;

		case MODE_5:
			SetLED(0x0e);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			MAP_showCmdLog();
			break;

		case MODE_6:
			SetLED(0x0e);
			break;

		case MODE_7:
			SetLED(0x0e);
			//cant use
			break;

		default:
			break;
	}
}

void MODE_exe_m2( void )
{
	enMAP_HEAD_DIR		en_endDir2;

	uint64_t data =0;
	Min_in = 0;
	Sec_in = 0;
	Msec_in = 0;

	GYRO_SetRef();
	CTRL_clrData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
	CTRL_clrNowData();

	switch( en_Mode ){

		case MODE_0://only adachi
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);						
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );							
*/
			SetLED(0x00);
			LL_mDelay(100);
			MAP_Goalsize(1);
			MAP_setPos( 0, 0, NORTH );						

			MAP_searchGoal(GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, SEARCH, SEARCH_SLA );			

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
			}
			
			SetLED(0x0e);
			MAP_Goalsize(1);
			SetLED(0x00);

			MAP_searchGoal( 0, 0, SEARCH, SEARCH_SLA );

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
				MAP_setPos( 0, 0, NORTH );								// スタート位置
				MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
				MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
				MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
				MAP_makeSkewCmdList();
				SetLED(0x00);
			}
			break;

		case MODE_1://adachi and known 
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);						
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );	
*/					
			SetLED(0x00);
			LL_mDelay(100);
			MAP_Goalsize(1);
			MAP_setPos( 0, 0, NORTH );							

			MAP_searchGoalKnown(GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, SEARCH, SEARCH_SLA );			

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
			}
			
			SetLED(0x0e);
			MAP_Goalsize(1);
			SetLED(0x00);
			log_flag_on();
			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_SLA );
//			MAP_searchGoal( 0, 0, SEARCH, SEARCH_RETURN );
			log_flag_off();
			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
				MAP_setPos( 0, 0, NORTH );								// スタート位置
				MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
				MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
				MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
				MAP_makeSkewCmdList();
				SetLED(0x00);
			}
			break;

		case MODE_2://adachi and known and return (cancel return now) 
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );	
*/						
			SetLED(0x00);
			LL_mDelay(100);
//			PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
			MAP_Goalsize(1);
			MAP_setPos( 0, 0, NORTH );							

			MAP_searchGoalKnown(GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, SEARCH, SEARCH_SLA );			

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
			}
			
			SetLED(0x0e);
			MAP_Goalsize(1);
			SetLED(0x00);

			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_SLA );
//			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_RETURN );

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
				MAP_setPos( 0, 0, NORTH );								// スタート位置
				MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
				MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
				MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
				MAP_makeSkewCmdList();
				SetLED(0x00);
			}
			break;

		case MODE_3:
			SetLED(0x0e);
			
			break;

		case MODE_4:
			SetLED(0x0e);

			break;

		case MODE_5://test search
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );							
*/			SetLED(0x00);
			LL_mDelay(100);
//			PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
			MAP_Goalsize(1);
			MAP_setPos( 0, 0, NORTH );							

			MAP_searchGoalKnown(2, 0, SEARCH, SEARCH_SLA );			

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
			}
			
			SetLED(0x0e);
			MAP_Goalsize(1);
			SetLED(0x00);

			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_SLA );
//			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_RETURN );

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
				MAP_setPos( 0, 0, NORTH );								// スタート位置
				MAP_makeContourMap_run( 2, 0, BEST_WAY );					// 等高線�?�ップを作る
				MAP_makeCmdList( 0, 0, NORTH,2, 0, &en_endDir2 );		// ドライブコマンド作�??
				MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
				MAP_makeSkewCmdList();
				SetLED(0x00);
			}
			break;

		case MODE_6://test
			printf("\r\n turn 45 \r\r");
			PARAM_makeSra( 0.5, 550.0f, 7.50f,3000*PI,  SLA_45 );
			printf("\r\n turn 90 \r\r");		
			PARAM_makeSra( 0.5, 700.0f, 10.00f,3000*PI,  SLA_90 );	
		//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
			printf("\r\n turn 135 \r\r");	
			PARAM_makeSra( 0.5, 750.0f, 10.00f,3000*PI,  SLA_135 );	
			printf("\r\n turn N90 \r\r");	
			PARAM_makeSra( 0.5, 800.0f, 11.00f,3000*PI,  SLA_N90 );	

			Map_Copy();

			GYRO_SetRef();
			CTRL_clrData();
			CTRL_clrAngleErrSum();
			CTRL_clrSpeedErrSum();
			CTRL_clrNowData();
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*4.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*4.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);								
			PARAM_setSpeedType( PARAM_ST,   PARAM_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );										
*/
			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( 2, 0, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH,  2, 0, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);			

			log_flag_on();										
			MAP_drive( MAP_DRIVE_SKEW );
			log_flag_off();
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();

			PARAM_makeSra( (float)SEARCH_SPEED, 100.0f, 2.50f,3000*PI,  SLA_45 );
			printf("\r\n turn 90 \r\r");		
			PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 3.50f,3000*PI,  SLA_90 );	
		//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
			printf("\r\n turn 135 \r\r");	
			PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 4.00f,3000*PI,  SLA_135 );	
			printf("\r\n turn N90 \r\r");	
			PARAM_makeSra( (float)SEARCH_SPEED, 300.0f, 4.00f,3000*PI,  SLA_N90 );		
			MODE_speed_parameter(PARAM_VERY_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);
*/
			break;

		case MODE_7:
			SetLED(0x0e);
			//cant use
			break;

		default:
			break;
	}
}

void MODE_exe_m3( void )
{
	enMAP_HEAD_DIR		en_endDir2;

	uint64_t data =0;

	Map_Copy();

	GYRO_SetRef();
	CTRL_clrData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
	CTRL_clrNowData();

	switch( en_Mode ){

		case MODE_0:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED*4.0,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*4.0);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );							
*/			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);
			MAP_drive( MAP_DRIVE_SLA );													
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_1:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED*5.0,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*5.0);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );								
*/			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);													
			MAP_drive( MAP_DRIVE_SLA );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_2:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_NORMAL,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED*6.0,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*6.0);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );								
*/			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);												
			MAP_drive( MAP_DRIVE_SLA );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);			
			Failsafe_flag_off();
			break;

		case MODE_3:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED*4.0,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*4.0);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);						
			PARAM_setSpeedType( PARAM_ST,   PARAM_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );								
*/			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);		
			
			log_flag_on();
			MAP_drive( MAP_DRIVE_SKEW );

			log_flag_off();
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_4:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED*5.0,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*5.0);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );										
*/			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);		
			log_flag_on();											
			MAP_drive( MAP_DRIVE_SKEW );
			log_flag_off();
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_5:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_FAST,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED*6.0,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*6.0);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );									
*/			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);													
			MAP_drive( MAP_DRIVE_SKEW );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_6:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_SLOW,PARAM_VERY_SLOW,PARAM_SLOW,SEARCH_SPEED*5.0,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*5.0);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_SLOW );									
*/			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置

			MAP_Goal_init();
			MAP_makeContourMap_dijkstra_modoki(GOAL_MAP_X_DEF,GOAL_MAP_Y_DEF, BEST_WAY);
			MAP_Goalsize(1);
	
			MAP_makeCmdList_dijkstra_modoki(0, 0, NORTH, GOAL_MAP_X_DEF,GOAL_MAP_Y_DEF, &en_endDir2);		// ドライブコマンド作成
			MAP_makeSlaCmdList();													// スラロームコマンド作成
			MAP_makeSkewCmdList();

			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);													
			MAP_drive( MAP_DRIVE_SKEW );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();

			break;

		case MODE_7:
			SetLED(0x0e);
			//cant use
			break;

		default:
			break;
	}
}

void MODE_exe_m4( void )
{
	enMAP_HEAD_DIR		en_endDir2;

	uint64_t data =0;
	Min_in = 0;
	Sec_in = 0;
	Msec_in = 0;

	GYRO_SetRef();
	CTRL_clrData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
	CTRL_clrNowData();

	switch( en_Mode ){

		case MODE_0:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );								
*/			SetLED(0x00);
			LL_mDelay(100);
			MAP_Goalsize(1);
			MAP_setPos( 0, 0, NORTH );						

			MAP_searchGoal(GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, SEARCH, SEARCH_SLA );			

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
			}
			
			SetLED(0x0e);
			MAP_Goalsize(1);
			SetLED(0x00);

			MAP_searchGoal( 0, 0, SEARCH, SEARCH_SLA );

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
				MAP_setPos( 0, 0, NORTH );								// スタート位置
				MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
				MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
				MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
				MAP_makeSkewCmdList();
				SetLED(0x00);
			}
			break;

		case MODE_1:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );							
*/			SetLED(0x00);
			LL_mDelay(100);
			MAP_Goalsize(1);
			MAP_setPos( 0, 0, NORTH );							

			MAP_searchGoalKnown(GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, SEARCH, SEARCH_SLA );			

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
			}
			
			SetLED(0x0e);
			MAP_Goalsize(1);
			SetLED(0x00);

			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_SLA );
//			MAP_searchGoal( 0, 0, SEARCH, SEARCH_RETURN );

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
				MAP_setPos( 0, 0, NORTH );								// スタート位置
				MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
				MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
				MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
				MAP_makeSkewCmdList();
				SetLED(0x00);
			}
			break;

		case MODE_2:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_SLOW,PARAM_VERY_SLOW,PARAM_VERY_SLOW,SEARCH_SPEED,
								SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED,SEARCH_SPEED);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_45);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_135);
			MOT_setSlaStaSpeed( SEARCH_SPEED , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_VERY_SLOW );							
*/			SetLED(0x00);
			LL_mDelay(100);
//			PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
			MAP_Goalsize(1);
			MAP_setPos( 0, 0, NORTH );							

			MAP_searchGoalKnown(GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, SEARCH, SEARCH_SLA );			

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
			}
			
			SetLED(0x0e);
			MAP_Goalsize(1);
			SetLED(0x00);

			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_SLA );
//			MAP_searchGoalKnown( 0, 0, SEARCH, SEARCH_RETURN );

			if (( SW_IsOn_1() == SW_ON)||(SYS_isOutOfCtrl() == TRUE)){}
			else{
				Map_Write();
				MAP_setPos( 0, 0, NORTH );								// スタート位置
				MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
				MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
				MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
				MAP_makeSkewCmdList();
				SetLED(0x00);
			}
			break;

		case MODE_3:
			SetLED(0x0e);
			
			break;

		case MODE_4:
			SetLED(0x0e);
			break;

		case MODE_5:
			SetLED(0x0e);
			break;

		case MODE_6:
			SetLED(0x0e);
			break;

		case MODE_7:
			SetLED(0x0e);
			//cant use
			break;

		default:
			break;
	}
}

void MODE_exe_m5( void )
{
	enMAP_HEAD_DIR		en_endDir2;

	uint64_t data =0;

	printf("\r\n turn 45 \r\r");
	PARAM_makeSra( 0.5, 550.0f, 7.50f,3000*PI,  SLA_45 );
	printf("\r\n turn 90 \r\r");		
	PARAM_makeSra( 0.5, 700.0f, 10.00f,3000*PI,  SLA_90 );	
//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
	printf("\r\n turn 135 \r\r");	
	PARAM_makeSra( 0.5, 750.0f, 10.00f,3000*PI,  SLA_135 );	
	printf("\r\n turn N90 \r\r");	
	PARAM_makeSra( 0.5, 800.0f, 11.00f,3000*PI,  SLA_N90 );	

	Map_Copy();

	GYRO_SetRef();
	CTRL_clrData();
	CTRL_clrAngleErrSum();
	CTRL_clrSpeedErrSum();
	CTRL_clrNowData();

	switch( en_Mode ){

		case MODE_0:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*4.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*4.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							
*/			SetLED(0x00);	
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);	
			MAP_drive( MAP_DRIVE_SLA );												
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_1:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*5.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*5.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							
*/								
			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);													
			MAP_drive( MAP_DRIVE_SLA );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_2:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*6.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*6.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							
*/								
			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);												
			MAP_drive( MAP_DRIVE_SLA );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);			
			Failsafe_flag_off();
			break;

		case MODE_3:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*4.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*4.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							
*/								
			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);			

			log_flag_on();										
			MAP_drive( MAP_DRIVE_SKEW );
			log_flag_off();
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_4:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*5.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*5.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							
*/							
			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);													
			MAP_drive( MAP_DRIVE_SKEW );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_5:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*6.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*6.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							
*/										
			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置
			MAP_Goalsize(1);
			MAP_makeContourMap_run( GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, BEST_WAY );					// 等高線�?�ップを作る
			MAP_makeCmdList( 0, 0, NORTH, GOAL_MAP_X_DEF, GOAL_MAP_Y_DEF, &en_endDir2 );		// ドライブコマンド作�??
			MAP_makeSlaCmdList();													// スラロー�?コマンド作�??
			MAP_makeSkewCmdList();													
			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);													
			MAP_drive( MAP_DRIVE_SKEW );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_6:
			SetLED(0x0e);
			MODE_speed_parameter(PARAM_VERY_FAST,PARAM_VERY_SLOW,PARAM_NORMAL,SEARCH_SPEED*6.0,
								0.5,0.5,0.5,0.5);
/*
			MOT_setTrgtSpeed(SEARCH_SPEED*6.0);
			MOT_setSlaStaSpeed( 0.5 , SLA_90);							// スラロー�?開始速度設�?
			MOT_setSlaStaSpeed( 0.5 , SLA_45);
			MOT_setSlaStaSpeed( 0.5 , SLA_135);
			MOT_setSlaStaSpeed( 0.5 , SLA_N90);							
			PARAM_setSpeedType( PARAM_ST,   PARAM_VERY_FAST );							
			PARAM_setSpeedType( PARAM_TRUN, PARAM_VERY_SLOW );							
			PARAM_setSpeedType( PARAM_SLA,  PARAM_NORMAL );							
*/									
			SetLED(0x00);
			MAP_setPos( 0, 0, NORTH );												// スタート位置

			MAP_Goal_init();
			MAP_makeContourMap_dijkstra_modoki(GOAL_MAP_X_DEF,GOAL_MAP_Y_DEF, BEST_WAY);
			MAP_Goalsize(1);
	
			MAP_makeCmdList_dijkstra_modoki(0, 0, NORTH, GOAL_MAP_X_DEF,GOAL_MAP_Y_DEF, &en_endDir2);		// ドライブコマンド作成
			MAP_makeSlaCmdList();													// スラロームコマンド作成
			MAP_makeSkewCmdList();

			LL_mDelay(500);
			Set_DutyTIM8(600);
			LL_mDelay(2000);													
			MAP_drive( MAP_DRIVE_SKEW );
			Set_DutyTIM8(0);
			LL_mDelay(500);
			MOT_turn(MOT_R180);
			MAP_actGoalLED();
			Set_DutyTIM8(0);
			Failsafe_flag_off();
			break;

		case MODE_7:
			SetLED(0x0e);
			//cant use
			break;

		default:
			break;
	}
	printf("\r\n turn 45 \r\r");
	PARAM_makeSra( (float)SEARCH_SPEED, 100.0f, 2.50f,3000*PI,  SLA_45 );
	printf("\r\n turn 90 \r\r");		
	PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 3.50f,3000*PI,  SLA_90 );	
//	PARAM_makeSra( (float)SEARCH_SPEED, 150.0f, 3.00f,3000*PI,  SLA_90 );
	printf("\r\n turn 135 \r\r");	
	PARAM_makeSra( (float)SEARCH_SPEED, 200.0f, 4.00f,3000*PI,  SLA_135 );	
	printf("\r\n turn N90 \r\r");	
	PARAM_makeSra( (float)SEARCH_SPEED, 300.0f, 4.00f,3000*PI,  SLA_N90 );	
}


void MODE_exe( void )
{
//	uint16_t *read;
	enMAP_HEAD_DIR		en_endDir;

	NowModeLed = MODELED_1;
	uint8_t i;
	LL_mDelay(300);
	GYRO_SetRef();
	ENC_clr();
	Failsafe_flag_off();
//	log_flag_on();	

	switch( en_Mode ){

		case MODE_0://tune	
			SetLED(0x0e);
			en_Mode = MODE_0;	
			LL_mDelay(100);
			SetLED(0x00);
			NowModeLed = MODELED_2;
			while(1){
				if (( SW_IsOn_1() == SW_ON)||CountUP_mode()){
					MODE_inc();								
					LL_mDelay(200);			
					printf("mode selecting_0\r\n");
				}
				else if (( SW_IsOn_0() == SW_ON )||(TRUE == MODE_CheckExe())){
					MODE_exe_m0();								
					LL_mDelay(200);				
					if (en_Mode == MODE_7){
						NowModeLed = MODELED_1;
						break;
					}
				}

			}
			en_Mode = MODE_0;
			break;

		case MODE_1://flash
			SetLED(0x0e);
			en_Mode = MODE_0;	
			LL_mDelay(100);
			SetLED(0x00);
			NowModeLed = MODELED_2;
			while(1){
				if (( SW_IsOn_1() == SW_ON)||CountUP_mode()){
					MODE_inc();								
					LL_mDelay(200);			
					printf("mode selecting_1\r\n");
				}
				else if (( SW_IsOn_0() == SW_ON )||(TRUE == MODE_CheckExe())){
					MODE_exe_m1();								
					LL_mDelay(200);				
					if (en_Mode == MODE_7){
						NowModeLed = MODELED_1;
						break;
					}
				}

			}
			en_Mode = MODE_1;
			break;

		case MODE_2://search wall hit
			SetLED(0x0e);
			en_Mode = MODE_0;	
			LL_mDelay(100);
			SetLED(0x00);
			NowModeLed = MODELED_2;
			while(1){
				if (( SW_IsOn_1() == SW_ON)||CountUP_mode()){
					MODE_inc();								
					LL_mDelay(200);			
					printf("mode selecting_1\r\n");
				}
				else if (( SW_IsOn_0() == SW_ON )||(TRUE == MODE_CheckExe())){
					MODE_exe_m2();								
					LL_mDelay(200);				
					if (en_Mode == MODE_7){
						NowModeLed = MODELED_1;
						break;
					}
				}

			}
			en_Mode = MODE_2;
			break;

		case MODE_3://drive
			SetLED(0x0e);
			en_Mode = MODE_0;	
			LL_mDelay(100);
			SetLED(0x00);
			NowModeLed = MODELED_2;
			while(1){
				if (( SW_IsOn_1() == SW_ON)||CountUP_mode()){
					MODE_inc();								
					LL_mDelay(200);			
					printf("mode selecting_1\r\n");
				}
				else if (( SW_IsOn_0() == SW_ON )||(TRUE == MODE_CheckExe())){
					MODE_exe_m3();								
					LL_mDelay(200);				
					if (en_Mode == MODE_7){
						NowModeLed = MODELED_1;
						break;
					}
				}

			}
			en_Mode = MODE_3;
			break;

		case MODE_4: // wall hit search
			SetLED(0x0e);
			en_Mode = MODE_0;	
			LL_mDelay(100);
			SetLED(0x00);
			NowModeLed = MODELED_2;
			WallHitFlag = 1;
			while(1){
				if (( SW_IsOn_1() == SW_ON)||CountUP_mode()){
					MODE_inc();								
					LL_mDelay(200);			
					printf("mode selecting_1\r\n");
				}
				else if (( SW_IsOn_0() == SW_ON )||(TRUE == MODE_CheckExe())){
					MODE_exe_m4();								
					LL_mDelay(200);				
					if (en_Mode == MODE_7){
						NowModeLed = MODELED_1;
						WallHitFlag = 0;
						break;
					}
				}

			}
			en_Mode = MODE_4;
			break;

		case MODE_5:
			SetLED(0x0e);
			en_Mode = MODE_0;	
			LL_mDelay(100);
			SetLED(0x00);
			NowModeLed = MODELED_2;
			while(1){
				if (( SW_IsOn_1() == SW_ON)||CountUP_mode()){
					MODE_inc();								
					LL_mDelay(200);			
					printf("mode selecting_1\r\n");
				}
				else if (( SW_IsOn_0() == SW_ON )||(TRUE == MODE_CheckExe())){
					MODE_exe_m5();								
					LL_mDelay(200);				
					if (en_Mode == MODE_7){
						NowModeLed = MODELED_1;
						break;
					}
				}

			}
			en_Mode = MODE_5;
			break;

		case MODE_6:
			SetLED(0x0e);

			break;

		case MODE_7:
			SetLED(0x0e);
			
			break;


		default:
			break;
	}
}

bool MODE_DistRightCheck(void)
{
	int16_t s_rightval;
	bool bl_check;

	s_rightval = DIST_getNowVal(DIST_SEN_R_FRONT);

	if( s_rightval >= 300 ){
		bl_check=TRUE;
	}
	else{
		bl_check=FALSE;
	}

	return bl_check;
}

bool MODE_DistLeftCheck(void){

	int16_t 	s_leftval;
	bool	bl_check;

	s_leftval 	= DIST_getNowVal(DIST_SEN_L_FRONT);

	if( s_leftval >= 200 ){
		bl_check = TRUE;

	}else{
		bl_check = FALSE;

	}

	return bl_check;
}

bool MODE_setWaitCheck(void){

	bool bl_check;

	if( TRUE == MODE_DistRightCheck() ){	// 右�?け検知
		SetLED(0x08);
	}
	if( TRUE == MODE_DistLeftCheck() ){		// 左�?け検知
		SetLED(0x02);
	}

	if( ( TRUE == MODE_DistRightCheck() ) && ( TRUE == MODE_DistLeftCheck() ) ){
		SetLED(0x0e);
		bl_check = TRUE;

	}else{
		bl_check = FALSE;
	}
	return bl_check;
}

bool MODE_CheckExe(void){

	bool bl_check;

	if( TRUE == MODE_setWaitCheck() ){
		LL_mDelay(500);

		if( FALSE == MODE_setWaitCheck() ){
			SetLED(0x00);
			LL_mDelay(1000);
			bl_check = TRUE;

		}else{
			bl_check = FALSE;

		}

	}else{

		bl_check = FALSE;
	}

	return bl_check;
}


