/*
 * encoder.h
 *
 *  Created on: Jan 13, 2021
 *      Author: shohe
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#ifdef _VSCODE
#include "Inc/glob.h"
#include "Inc/gpio.h"
#include "Inc/hal/CTRL.h"
#include "Inc/hal/run.h"
#else
#include "glob.h"
#include "gpio.h"
#include "hal/CTRL.h"
#include "hal/run.h"
#endif

typedef enum{
    EN_L=0,
    EN_R,
    ENDIR_MAX
}en_ENDIR;


void ENC_clr( void );
uint16_t Get_encoder_value(en_ENDIR dir);
void ENC_GetDiv( int32_t* p_r, int32_t* p_l );
void ENC_print(void);
void recv_spi_encoder(en_ENDIR dir);
void Set_encoder_data(en_ENDIR dir);
#endif /* INC_ENCODER_H_ */
