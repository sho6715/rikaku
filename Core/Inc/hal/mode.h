/*
 * mode.h
 *
 *  Created on: 2021/01/30
 *      Author: shohe
 */

#ifndef INC_MODE_H_
#define INC_MODE_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#include "Inc/hal/CTRL.h"
#include "Inc/hal/DCM.h"
#include "Inc/hal/encoder.h"
#include "Inc/hal/gyro.h"
#include "Inc/hal/init.h"
#include "Inc/hal/log.h"
#include "Inc/hal/run.h"
#include "Inc/hal/sen_batt.h"
#include "Inc/hal/sen_dist.h"
#include "Inc/search/map_cmd.h"
#include "Inc/TIM.h"
#else
#include "glob.h"
#include "hal/CTRL.h"
#include "hal/DCM.h"
#include "hal/encoder.h"
#include "hal/gyro.h"
#include "hal/init.h"
#include "hal/log.h"
#include "hal/run.h"
#include "hal/sen_batt.h"
#include "hal/sen_dist.h"
#include "search/map_cmd.h"
#include "TIM.h"
#endif

void SYS_start( void );
void MODE_inc( void );
bool CountUP_mode(void);
void MODE_exe( void );
bool MODE_CheckExe(void);

#endif /* INC_MODE_H_ */
