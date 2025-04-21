/*
 * time_profile.c
 *
 *  Created on: Apr 21, 2025
 *      Author: prasa
 */


#include "time_profile.h"

static uint32_t cycle_start = 0;

void profiler_init(void)
{
    // Enable the DWT and the cycle counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   // Enable DWT access
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;              // Enable the counter
    DWT->CYCCNT = 0;                                   // Reset the counter
}

void profiler_start(void)
{
    cycle_start = DWT->CYCCNT;
}

uint32_t profiler_stop_cycles(void)
{
    return DWT->CYCCNT - cycle_start;
}

// Integer microseconds version
uint32_t profiler_stop_us(void)
{
    uint32_t cycles = profiler_stop_cycles();
    uint32_t cpu_freq = HAL_RCC_GetHCLKFreq(); // e.g., 160000000 Hz

    // Multiply first to preserve precision before dividing
    return (cycles * 1000000U) / cpu_freq; // result in microseconds
}

// Returns time in nanoseconds (64-bit)
uint64_t profiler_stop_ns(void)
{
    uint32_t cycles = profiler_stop_cycles();
    uint32_t cpu_freq = HAL_RCC_GetHCLKFreq();  // e.g., 160 MHz

    return ((uint64_t)cycles * 1000000000ULL) / cpu_freq;
}
