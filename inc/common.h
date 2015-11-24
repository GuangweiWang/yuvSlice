//
//  common.h
//  ImageProcess
//
//  Created by Guangwei Wang on 11/17/15.
//  Copyright © 2015 Guangwei Wang. All rights reserved.
//

#ifndef _COMMON_H_
#define _COMMON_H_

//define macros
#ifndef MY_ROUND
#define MY_ROUND(x) ((int32_t)(0.5+(x)))
#endif//WELS_ROUND

#define MY_MAX(x, y) ((x) > (y) ? (x) : (y))
#define MY_MIN(x, y) ((x) < (y) ? (x) : (y))

#define MY_CLAMP(x, minv, maxv)  WELS_MIN(WELS_MAX(x, minv), maxv)

#define MY_ABS(x) (x) < 0 ? (-(x)):(x)


#endif /* common_h */
