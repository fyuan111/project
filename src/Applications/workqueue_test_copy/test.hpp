#pragma once

#include <platform/apps.h>
#include <shared_thread/workitem.hpp>
#include <math.h>

/**
 * @brief 工作队列测试应用
 */
class workqueue_test_copy : public fml::AppBase<workqueue_test_copy>, public WorkItem
{
public:
    workqueue_test_copy() : fml::AppBase<workqueue_test_copy>("workqueue_test_copy"), WorkItem(2, 1500) // 1.5ms = 15 * 100us
    {
    }

    ~workqueue_test_copy();

    void run() override;

private:
    // 模拟真实飞控负载（无除法，避免性能瓶颈）
    inline void simulate_workload(uint32_t load_level)
    {
        volatile uint32_t result = 0;
        for (uint32_t i = 0; i < load_level; i++)
        {
            // 模拟传感器数据处理：乘法、加法、位运算
            result += (i * 17) ^ (i >> 3);           // 乘法 + 移位 + 异或
            result = (result * 13) + (i << 2);       // 乘法 + 移位 + 加法（去掉除法！）
            result ^= (result << 5) | (result >> 7); // 位运算
        }
    }

private:
    uint64_t last_time_us{0};
    uint32_t count{0};
    uint64_t max_diff{0};
    uint64_t min_diff{UINT64_MAX};
    uint64_t sum_diff{0};
    uint64_t start_time_us{0};    // 开始时间
    uint32_t total_executions{0}; // 总执行次数

    // 执行时间统计
    uint64_t max_exec_time{0};
    uint64_t min_exec_time{UINT64_MAX};
    uint64_t sum_exec_time{0};
    uint32_t timeout_count{0}; // 执行时间超过周期的次数
};
