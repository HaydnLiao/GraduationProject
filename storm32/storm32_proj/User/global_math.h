
#ifndef __GLOBAL_MATH_H
#define __GLOBAL_MATH_H

#include <math.h>

#define SYSTEM_PERIOD		((uint8_t)(10))	//unit: ms
#define MATH_PI				((double)(3.141592653589793))
#define INTERVAL_CONSTRAINT(x, max, min)	((x)>(max)?(max):((x)<(min)?(min):(x)))

#endif
