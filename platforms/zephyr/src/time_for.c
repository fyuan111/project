#include <zephyr/timing/timing.h>
#include <platform/time.h>
#include <zephyr/kernel.h>

f_abstime_t f_time_duration_ns(f_abstime_t start, f_abstime_t end)
{
#ifdef CONFIG_TIMING_FUNCTIONS
    return timing_cycles_to_ns(timing_cycles_get(&start, &end));
#else
    printk("CONFIG_TIMING_FUNCTIONS not enabled");
    return 0;
#endif
}

f_abstime_t f_time_us()
{
    uint32_t cycles = f_cycle_get_32();
    return (uint64_t)cycles * 1000000ULL / CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;
}