
#ifndef __GLOBAL_MATH_H
#define __GLOBAL_MATH_H

#include <math.h>

#define SYSTEM_PERIOD		((10))	//unit: ms
#define MATH_PI				((3.141592653589793))
#define INTERVAL_CONSTRAINT(x, max, min)	((x)>(max)?(max):((x)<(min)?(min):(x)))

#endif
