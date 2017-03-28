
#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"
#include "global_math.h"

static float pitchPPara, pitchIPara, pitchDPara;
static float rollPPara, rollIPara, rollDPara;
static float yawPPara, yawIPara, yawDPara;

#define PITCH_P_PARAM		((float)(1.0))
#define PITCH_I_PARAM		((float)(0.0))
#define PITCH_D_PARAM		((float)(0.0))

#define ROLL_P_PARAM		((float)(1.0))
#define ROLL_I_PARAM		((float)(0.0))
#define ROLL_D_PARAM		((float)(0.0))

#define YAW_P_PARAM			((float)(1.0))
#define YAW_I_PARAM			((float)(0.0))
#define YAW_D_PARAM			((float)(0.0))

#define DEBUG_USART1_PID	1
#define DEBUG_PARA_LEN		9
static float debugPara[DEBUG_PARA_LEN];

float PID_Motor0(float actAngle, float expAngle);
float PID_Motor1(float actAngle, float expAngle);
float PID_Motor2(float actAngle, float expAngle);
void Usart1StringToFloat(void);

#endif

