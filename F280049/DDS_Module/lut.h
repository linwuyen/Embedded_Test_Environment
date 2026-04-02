/*
 * Lut.h
 *
 *  Created on: Nov 18, 2025
 *      Author: roger_lin
 */

#ifndef LUT_H_
#define LUT_H_

#include <stdint.h> // For int16_t, int32_t

#define LUT_SIZE 1024  // Reduced from 4096 to fit in Flash (8KB total for 2 tables)



extern const uint32_t g_SinLUT_Q24[LUT_SIZE];
extern const uint32_t g_TeapLUT_Q24[LUT_SIZE];



#endif /* LUT_H_ */
