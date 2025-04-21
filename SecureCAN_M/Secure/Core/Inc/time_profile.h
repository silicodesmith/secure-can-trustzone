/*
 * time_profile.h
 *
 *  Created on: Apr 21, 2025
 *      Author: prasa
 */

#ifndef INC_TIME_PROFILE_H_
#define INC_TIME_PROFILE_H_

#include "stm32h5xx_hal.h"

void profiler_init(void);
void profiler_start(void);
uint32_t profiler_stop_cycles(void);
uint32_t profiler_stop_us(void);  // Returns time in microseconds (uint32_t)
uint64_t profiler_stop_ns(void); // Returns time in nanoseconds (uint64_t)

#endif /* INC_TIME_PROFILE_H_ */
