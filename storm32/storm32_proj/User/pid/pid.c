
#include "pid.h"
#include "usart1.h"
#include "stdio.h"

float PID_Motor0(float actAngle, float expAngle)		//actual angle and expected angle; return speed
{
	static float expAngle_old = 0, et_pre = 0, et_sum = 0, et_diff = 0;
	float et = 0, dpid = 0;

#if DEBUG_USART1_PID
	//usart1-debug-pid parameters
	pitchPPara = debugPara[0];
	pitchIPara = debugPara[1];
	pitchDPara = debugPara[2];
	//printf("[0]P:%f\tI:%f\tD:%f\r\n", pitchPPara, pitchIPara, pitchDPara);
#endif

	if(expAngle_old != expAngle)
	{
		expAngle_old = expAngle;
		et_pre = et_sum = et_diff = 0;
	}
	et = actAngle - expAngle;
	et_sum += et;
	et_diff = et - et_pre;
	dpid = pitchPPara * et + pitchIPara * et_sum + pitchDPara * et_diff;
	//deadband
	
	//limit

	return dpid;
}

float PID_Motor1(float actAngle, float expAngle)		//actual angle and expected angle; return speed
{
	static float expAngle_old = 0, et_pre = 0, et_sum = 0, et_diff = 0;
	float et = 0, dpid = 0;

#if DEBUG_USART1_PID
	//usart1-debug-pid parameters
	rollPPara = debugPara[3];
	rollIPara = debugPara[4];
	rollDPara = debugPara[5];
	//printf("[1]P:%f\tI:%f\tD:%f\r\n", rollPPara, rollIPara, rollDPara);
#endif

	if(expAngle_old != expAngle)
	{
		expAngle_old = expAngle;
		et_pre = et_sum = et_diff = 0;
	}
	et = actAngle - expAngle;
	et_sum += et;
	et_diff = et - et_pre;
	dpid = rollPPara * et + rollIPara * et_sum + rollDPara * et_diff;
	//deadband
	
	//limit

	return dpid;
}

float PID_Motor2(float actAngle, float expAngle)		//actual angle and expected angle; return speed
{
	static float expAngle_old = 0, et_pre = 0, et_sum = 0, et_diff = 0;
	float et = 0, dpid = 0;

#if DEBUG_USART1_PID
	//usart1-debug-pid parameters
	yawPPara = debugPara[6];
	yawIPara = debugPara[7];
	yawDPara = debugPara[8];
	//printf("[2]P:%f\tI:%f\tD:%f\r\n", yawPPara, yawIPara, yawDPara);
#endif

	if(expAngle_old != expAngle)
	{
		expAngle_old = expAngle;
		et_pre = et_sum = et_diff = 0;
	}
	et = actAngle - expAngle;
	et_sum += et;
	et_diff = et - et_pre;
	dpid = yawPPara * et + yawIPara * et_sum + yawDPara * et_diff;
	//deadband
	
	//limit

	return dpid;
}

void Usart1StringToFloat(void)
{
	uint8_t tmpData = 0, pointFlag = 0;
	uint16_t cntU = 0, cntD = 0, cntPoint = 0;
	uint32_t tmpRst = 0;
	for(; cntU < cntUsart1RxBuff && cntD < DEBUG_PARA_LEN; cntU++)
	{
		tmpData = usart1RxBuff[cntU];
		if(tmpData == '\r' || tmpData == '\n')
		{
			cntUsart1RxBuff = 0;
			break;
		}
		else if(tmpData == '.')
		{
			pointFlag = 1;
		}
		else if(tmpData == ' ')
		{
			debugPara[cntD] = tmpRst * 1.0 / pow(10, cntPoint);
			cntD += 1;
			pointFlag = 0;
			cntPoint = 0;
			tmpRst = 0;
		}
		else
		{
			tmpRst = tmpRst*10 + (tmpData-'0');
			if(pointFlag == 1)
			{
				cntPoint += 1;
			}
		}
	}
}

