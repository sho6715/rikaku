/*
 * glob.h
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#ifndef INC_GLOB_H_
#define INC_GLOB_H_

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define SW_ON	(1)
#define SW_OFF	(0)
#define TRUE	(1)
#define FALSE	(0)
/* ăăŻă­ */
#define FABS(x)						( (x)>=0 ? (x) : -(x) )

#define GOAL_MAP_X_DEF					( 8 )//2				
#define GOAL_MAP_Y_DEF					( 7 )//11				
#define MAP_X_SIZE					( 32 )				
#define MAP_Y_SIZE					( 32 )				

#define MAP_X_SIZE_REAL				( 32 )					
#define MAP_Y_SIZE_REAL				( 32 )					

#define PI							( 3.14159f )								

#define VCC_MAX						( 4.2f )							
#define TIRE_D						( 0.01257f )	
#define TREAD_IMAGIN					(0.037f)
#define TREAD						( 0.035f )
#define ROTATE_PULSE				( 4096 )									
#define DIST_1STEP					( PI * TIRE_D / ROTATE_PULSE )			
#define F_CNT2MM(cnt)				( (float)cnt * DIST_1STEP )				
#define GEAR_RATIO					(37.0f/8.0f)

#define WEIGHT						(0.0214f)
#define INERTIA						(1.4f/100000.0f)
#define TORQUE_CONSTANT				(0.41f/1000.0f)
#define MOTOR_REGISTER				(2.3f)

#define DEG_TO_RAD  (3.1416f/180.0f)
#define RAD_TO_DEG  (180.0f/3.1416f)

typedef enum{
	DIST_SEN_R_FRONT = 0,
	DIST_SEN_L_FRONT,
	DIST_SEN_R_SIDE,
	DIST_SEN_L_SIDE,
	DIST_SEN_NUM
}enDIST_SEN_ID;

/* ă˘ăźăżID */
typedef enum{
	DCM_R = 0,					// ĺłă˘ăźăż
	DCM_L,						// ĺˇŚă˘ăźăż
	DCM_MAX
}enDCM_ID;

/* ćšĺ? */
typedef enum{
	NORTH =0,
	EAST,
	SOUTH,
	WEST,
	MAP_HEAD_DIR_MAX,
}enMAP_HEAD_DIR;

extern volatile uint16_t	Msec_in;
extern volatile uint8_t		Sec_in;	
extern volatile uint8_t		Min_in;	


#endif /* INC_GLOB_H_ */
