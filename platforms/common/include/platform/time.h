#pragma once

#include "define.h"
/**
 * @brief get and set current time from given timer
 *
 */
#include <unistd.h>

BEGIN_DECLS

#define f_clock_gettime sys_clock_gettime
#define f_clock_settime sys_clock_settime

#if defined(FML_ZEPHYR)
#include <zephyr/timing/timing.h>

#define f_cycle_get_32 k_cycle_get_32
#define f_cycle_get_64 k_cycle_get_64
#define f_clock_tick_get_32 sys_clock_tick_get_32
#define f_clock_tick_get_64 sys_clock_tick_get

#define f_time_init timing_init
#define f_time_get timing_counter_get
#define f_time_start timing_start
#define f_time_stop timing_stop

#elif defined(FML_LINUX)

#endif
/* cpu clock cycle */

f_abstime_t f_time_duration_ns(f_abstime_t start, f_abstime_t end);
f_abstime_t f_time_us();

END_DECLS


