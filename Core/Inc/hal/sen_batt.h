/*
 * sen_batt.h
 *
 *  Created on: 2021/01/13
 *      Author: shohe
 */

#ifndef INC_SEN_BATT_H_
#define INC_SEN_BATT_H_

#ifdef _VSCODE
#include "Inc/adc.h"
#include "Inc/gpio.h"
#include "Inc/glob.h"
#else
#include "adc.h"
#include "gpio.h"
#include "glob.h"
#endif

void BAT_Pol( void );
float get_battLv(void);

#endif /* INC_SEN_BATT_H_ */
