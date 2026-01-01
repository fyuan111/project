#include "test.hpp"
#include <platform/log.h>
#include <platform/common/atomic.hpp>
#include <shared_thread/workqueue_manager.hpp>
#include <zephyr/kernel.h>
#include <zephyr/drivers/counter.h>

workqueue_test2::~workqueue_test2()
{
    fml_info("=== WorkQueue Test Copy Destroyed ===\n");
}

void workqueue_test2::run()
{
    // 模拟高频控制任务（姿态控制）
    const uint32_t LOAD_LEVEL = 1000; // 约180-200us
    simulate_workload(LOAD_LEVEL);

    uint64_t current_time = WorkQueueManager::get_hrt().get_current_us();
    uint64_t expected_period_us = cycle_in_100us() * 100;

    if (last_time_us > 0)
    {
        uint64_t time_diff = current_time - last_time_us;

        // 检测异常间隔（打印日志耗时导致的），跳过统计
        if (time_diff > expected_period_us * 15 / 10) // 超过1.5倍周期
        {
            last_time_us = current_time;
            return;
        }

        // 更新统计数据
        if (time_diff > max_diff)
            max_diff = time_diff;
        if (time_diff < min_diff)
            min_diff = time_diff;
        sum_diff += time_diff;
        count++;

        // 每10次打印一次统计（20ms * 10 = 200ms）
        if (count >= 10)
        {
            uint64_t avg_diff = sum_diff / count;
            int64_t avg_error = avg_diff - expected_period_us;
            int64_t max_error = max_diff - expected_period_us;
            int64_t min_error = min_diff - expected_period_us;

            fml_info("3333:%d\n", get_tid());
            fml_info("[任务3] 10次统计 | 平均: %llu us (误差%lld) | 最大: %llu us (误差%lld) | 最小: %llu us (误差%lld)\n",
                     avg_diff, avg_error, max_diff, max_error, min_diff, min_error);

            // 重置统计
            count = 0;
            max_diff = 0;
            min_diff = UINT64_MAX;
            sum_diff = 0;
            // 注意：不return，让代码继续执行到最后更新last_time_us
        }
    }
    else
    {
        fml_info("[任务3] 首次执行 | 预期周期: %llu us (%.2f ms)\n",
                 expected_period_us, expected_period_us / 1000.0);
    }

    last_time_us = current_time;
}

FML_REGISTER_APP(workqueue_test2);
