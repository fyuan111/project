#include "test.hpp"
#include <platform/log.h>
#include <platform/common/atomic.hpp>
#include <shared_thread/workqueue_manager.hpp>
#include <zephyr/kernel.h>
#include <zephyr/drivers/counter.h>

workqueue_test::~workqueue_test()
{
    fml_info("=== WorkQueue Test Destroyed ===\n");
}

void workqueue_test::run()
{
    // // 模拟中等负载
    // const uint32_t LOAD_LEVEL = 2000; // 约360-400us
    // simulate_workload(LOAD_LEVEL);

    // uint64_t current_time = WorkQueueManager::get_hrt().get_current_us();
    // uint64_t expected_period_us = cycle_in_100us() * 100;

    // // 单次执行任务（事件驱动，cycle=0）
    // if (expected_period_us == 0)
    // {
    //     return;
    // }

    // if (last_time_us > 0)
    // {
    //     // 防止时间回绕导致下溢
    //     if (current_time < last_time_us)
    //     {
    //         fml_info("[任务1] 警告：时间戳回绕！current=%llu, last=%llu\n",
    //                  current_time, last_time_us);
    //         last_time_us = current_time;
    //         return;
    //     }

    //     uint64_t time_diff = current_time - last_time_us;

    //     // 更新统计数据
    //     if (time_diff > max_diff)
    //         max_diff = time_diff;
    //     if (time_diff < min_diff)
    //         min_diff = time_diff;
    //     sum_diff += time_diff;
    //     count++;

    //     // 每500次打印一次统计（20ms * 500 = 10秒）
    //     if (count >= 1000)
    //     {
    //         uint64_t avg_diff = sum_diff / count;
    //         int64_t avg_error = avg_diff - expected_period_us;
    //         int64_t max_error = max_diff - expected_period_us;
    //         int64_t min_error = min_diff - expected_period_us;

    //         fml_info("[任务1] 500次统计(10秒) | 平均: %llu us (误差%lld) | 最大: %llu us (误差%lld) | 最小: %llu us (误差%lld)\n",
    //                  avg_diff, avg_error, max_diff, max_error, min_diff, min_error);

    //         // 重置统计
    //         count = 0;
    //         max_diff = 0;
    //         min_diff = UINT64_MAX;
    //         sum_diff = 0;
    //     }
    // }
    // else
    // {
    //     fml_info("[任务1] 首次执行 | 预期周期: %llu us (%.2f ms)\n",
    //              expected_period_us, expected_period_us / 1000.0);
    // }

    // last_time_us = current_time;
    fml_info("1111:%d\n", get_tid());
}

FML_REGISTER_APP(workqueue_test);
