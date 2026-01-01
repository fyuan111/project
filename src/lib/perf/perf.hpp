
#pragma once

#include <platform/define.h>
#include <platform/log.h>
#include <stdint.h>

#ifdef FML_ZEPHYR
#include <zephyr/kernel.h>  // for CONFIG_* macros
#endif

// // 如果没有定义 CONFIG_FML_PERF，默认启用
// #ifndef CONFIG_FML_PERF
// #define CONFIG_FML_PERF 1
// #endif

// // 如果没有定义 CONFIG_FML_PERF_PRINT，默认启用
// #ifndef CONFIG_FML_PERF_PRINT
// #define CONFIG_FML_PERF_PRINT 1
// #endif

BEGIN_DECLS

#if defined(FML_ZEPHYR)
#include <zephyr/timing/timing.h>

#define  f_time_init              timing_init
#define  f_time_get               timing_counter_get
#define  f_time_start             timing_start
#define  f_time_stop              timing_stop
#define  f_time_duration_get(x,y) timing_duration_get(&x,&y)

inline uint64_t timing_duration_get(timing_t *x, timing_t *y)
{
    return timing_cycles_to_ns(timing_cycles_get(x,y));
}

#elif defined(FML_LINUX)
#include <time.h>

#define  f_time_init
#define  f_time_get(x)            clock_gettime(CLOCK_MONOTONIC,x)
#define  f_time_start
#define  f_time_stop
#define  f_time_duration_get(x,y) timing_duration_get(x,y)

#endif



END_DECLS

#if CONFIG_FML_PERF

class Perf
{
public:

    Perf()
    {
        // timing_init() 和 timing_start() 只需要调用一次，放在全局初始化
        // 每次构造都调用会重置定时器
        static bool timing_initialized = false;
        if (!timing_initialized) {
#ifdef FML_ZEPHYR
            timing_init();
            timing_start();
#endif
            timing_initialized = true;
        }
    }

    ~Perf() = default;

    void reset()
    {
        _start = 0;
        _end = 0;
        cur = 0;
        max = 0;
        min = UINT64_MAX;
        avg = 0;
        sum = 0;
        count = 0;
    }

    void start()
    {
        _start = timing_counter_get();
    }

    void stop()
    {
        _end = timing_counter_get();
        cur = timing_cycles_to_ns(timing_cycles_get(&_start, &_end));

        // fml_info("DEBUG: start=%llu, end=%llu, cur=%llu", _start, _end, cur);

        if(cur > max)
            max = cur;

        if(cur < min)
            min = cur;

        sum += cur;  // 累加
        count++;
        avg = sum / count;  // 真正的平均值
    }

    uint64_t get_count()
    {
        return count;
    }

    void print_runtime()
    {
        fml_info("count:%u,max:%llu,min:%llu,avg:%llu,cur:%llu\n",count,max,min,avg,cur);
    }

    void print_debug()
    {
        fml_info("DEBUG: start=%llu, end=%llu, cycles=%llu\n",
                 (unsigned long long)_start,
                 (unsigned long long)_end,
                 (unsigned long long)timing_cycles_get(&_start, &_end));
    }


private:
    timing_t _start{0}, _end{0};
    uint64_t cur{0};
    uint64_t max{0};
    uint64_t min{UINT64_MAX};
    uint64_t avg{0};
    uint64_t sum{0};  // 累加总和
    uint32_t count{0};

};

#else

class Perf
{
public:
    void start() {}
    void stop() {}
    void print_runtime() {}
};


#endif  // CONFIG_FML_PERF
