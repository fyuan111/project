#include "test.hpp"
#include <platform/log.h>
#include <platform/common/atomic.hpp>
#include <shared_thread/workqueue_manager.hpp>
#include <zephyr/kernel.h>
#include <zephyr/drivers/counter.h>

workqueue_test_copy::~workqueue_test_copy()
{
    fml_info("=== WorkQueue Test Copy Destroyed ===\n");
}

void workqueue_test_copy::run()
{
    uint64_t exec_start = WorkQueueManager::get_hrt().get_current_us();

    // ===== 负载测试配置 =====
    // 三任务压力测试
    // 任务3(prio0, 1ms): 1000 -> ~180us
    // 任务1(prio1, 1ms): 2000 -> ~360us
    // 任务2(prio1, 1.5ms): 2000 -> ~360us
    const uint32_t LOAD_LEVEL = 2000; // 修改这里调整负载

    simulate_workload(LOAD_LEVEL);

    uint64_t current_time = WorkQueueManager::get_hrt().get_current_us();
    uint64_t expected_period_us = cycle_in_100us() * 100;

    // 计算本次执行时间
    uint64_t exec_time = current_time - exec_start;
    if (exec_time > max_exec_time)
        max_exec_time = exec_time;
    if (exec_time < min_exec_time)
        min_exec_time = exec_time;
    sum_exec_time += exec_time;
    if (exec_time > expected_period_us)
        timeout_count++; // 执行超时

    if (last_time_us > 0)
    {
        // 防止时间回绕导致下溢
        if (current_time < last_time_us)
        {
            fml_info("[任务2] 警告:时间戳回绕！current=%llu, last=%llu, diff=%lld\n",
                     current_time, last_time_us, (int64_t)(current_time - last_time_us));
            last_time_us = current_time;
            return;
        }

        uint64_t time_diff = current_time - last_time_us;

        // 更新统计数据
        if (time_diff > max_diff)
            max_diff = time_diff;
        if (time_diff < min_diff)
            min_diff = time_diff;
        sum_diff += time_diff;
        count++;
        total_executions++;

        // 每6666次打印一次统计（1.5ms * 6666 ≈ 10秒）
        if (count >= 6666)
        {
            uint64_t avg_diff = sum_diff / count;
            int64_t avg_error = avg_diff - expected_period_us;
            int64_t max_error = max_diff - expected_period_us;
            int64_t min_error = min_diff - expected_period_us;

            uint64_t avg_exec_time = sum_exec_time / (count + 1); // +1因为包括首次执行

            // 理论上10秒应该执行6666次
            const uint32_t expected_executions = 6666;

            fml_info("2222:%d\n", get_tid());
            fml_info("[任务2] 负载级别: %u | 调度周期统计 | 平均: %llu us (误差%lld) | 最大: %llu us (误差%lld) | 最小: %llu us (误差%lld)\n",
                     LOAD_LEVEL, avg_diff, avg_error, max_diff, max_error, min_diff, min_error);
            fml_info("[任务2] 执行时间统计 | 平均: %llu us | 最大: %llu us | 最小: %llu us | 超时次数: %u (%.2f%%)\n",
                     avg_exec_time, max_exec_time, min_exec_time, timeout_count,
                     (timeout_count * 100.0f) / (count + 1));
            fml_info("[任务2] 执行次数 | 预期: %u 次 | 实际: %u 次 | 差值: %d 次\n",
                     expected_executions, total_executions, (int32_t)(total_executions - expected_executions));

            // 重置统计
            count = 0;
            total_executions = 0;
            max_diff = 0;
            min_diff = UINT64_MAX;
            sum_diff = 0;
            max_exec_time = 0;
            min_exec_time = UINT64_MAX;
            sum_exec_time = 0;
            timeout_count = 0;
        }
    }
    else
    {
        start_time_us = exec_start; // 记录开始时间（使用进入函数的时间）
        total_executions = 1;       // 第一次执行
        fml_info("[任务2] 首次执行 | 负载级别: %u | 预期周期: %llu us (%.2f ms) | 本次执行耗时: %llu us\n",
                 LOAD_LEVEL, expected_period_us, expected_period_us / 1000.0, exec_time);
    }

    last_time_us = current_time;
}

FML_REGISTER_APP(workqueue_test_copy);
