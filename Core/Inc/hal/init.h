/*
 * init.h
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#include "Inc/hal/DCM.h"
#include "Inc/hal/gyro.h"
#include "Inc/hal/CTRL.h"
#include "Inc/hal/sen_dist.h"
#include "Inc/search/search.h"
#include "Inc/hal/mode.h"
#include "Inc/tim.h"
#else
#include "glob.h"
#include "hal/DCM.h"
#include "hal/gyro.h"
#include "hal/CTRL.h"
#include "hal/sen_dist.h"
#include "search/search.h"
#include "hal/mode.h"
#include "tim.h"
#endif


void HAL_init( void );
void TIME_init( void );

#endif /* INC_INIT_H_ */
