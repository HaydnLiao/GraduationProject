
#include "pid.h"
#include "usart1.h"

float PID_Motor0(float actAngle, float expAngle)//actual angle and expected angle; return speed
{
	static uint16_t cntICal = 0;
	static float expAngle_old = 0.0, et_pre = 0.0, et_sum = 0.0, et_diff = 0.0;
	static float dP = 0.0, dI = 0.0, dD = 0.0;
	float et = 0.0, dpid = 0.0;

#if DEBUG_USART1_PID
	//usart1-debug-pid parameters
	pitchPPara = debugPara[0];
	pitchIPara = debugPara[1];
	pitchDPara = debugPara[2];
	printf("[0]P:%f I:%f D:%f \t", pitchPPara, pitchIPara, pitchDPara);
#endif

	if(expAngle_old != expAngle)
	{
		expAngle_old = expAngle;
		et_pre = et_sum = et_diff = 0;
	}

	et = actAngle - expAngle;
	if(fabs(et) < ET_CAL_DEADBAND)
	{
		et = 0.0;
	}
	dP = pitchPPara * et;
	//dP = INTERVAL_CONSTRAINT(dP, PITCH_P_UPPER, PITCH_P_LOWER);//proportion limit

	cntICal = (cntICal+1) % I_CAL_PERIOD;
	if(cntICal == 0)
	{
		et_sum += et;
		dI = pitchIPara * et_sum;
		dI = INTERVAL_CONSTRAINT(dI, PITCH_I_UPPER, PITCH_I_LOWER);//integration limit
	}

	et_diff = et - et_pre;
	dD = pitchDPara * et_diff;

	dpid = dP + dI + dD;
	//dpid = INTERVAL_CONSTRAINT(dpid, PITCH_RES_UPPER, PITCH_RES_LOWER);//result limit
	printf("et:%f et_sum:%f et_diff:%f dpid:%f \r\n", et, et_sum, et_diff, dpid);
	return dpid;
}

float PID_Motor1(float actAngle, float expAngle)		//actual angle and expected angle; return speed
{
	static float expAngle_old = 0.0, et_pre = 0.0, et_sum = 0.0, et_diff = 0.0;
	float et = 0.0, dpid = 0.0;

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
	//proportion limit
	
	et_sum += et;
	//integral limit
	
	et_diff = et - et_pre;
	dpid = rollPPara * et + rollIPara * et_sum + rollDPara * et_diff;
	//result limit
	

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
	//proportion limit
	
	et_sum += et;
	//integral limit
	
	et_diff = et - et_pre;
	dpid = yawPPara * et + yawIPara * et_sum + yawDPara * et_diff;
	//result limit
	

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

