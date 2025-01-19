/*
 * DCM.h
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#ifndef INC_DCM_H_
#define INC_DCM_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#include "Inc/tim.h"
#else
#include "glob.h"
#include "tim.h"
#endif


void DCM_setDirCw( enDCM_ID en_id );
void DCM_setDirCcw( enDCM_ID en_id );
void DCM_stopMot( enDCM_ID en_id );
void DCM_brakeMot( enDCM_ID en_id );
void DCM_staMot( enDCM_ID en_id );
void DCM_staMotAll( void );
void DCM_setPwmDuty( enDCM_ID en_id, uint16_t us_duty10 );


#endif /* INC_DCM_H_ */
