#pragma once

#include <platform/apps.h>
#include <shared_thread/workitem.hpp>

/**
 * @brief 工作队列测试应用
 */
class workqueue_test2 : public fml::AppBase<workqueue_test2>, public WorkItem
{
public:
    workqueue_test2() : fml::AppBase<workqueue_test2>("workqueue_test2"), WorkItem(0, 20000)
    {
    }

    ~workqueue_test2();

    void run() override;

private:
    // 模拟真实飞控负载（无除法）
    inline void simulate_workload(uint32_t load_level)
    {
        volatile uint32_t result = 0;
        for (uint32_t i = 0; i < load_level; i++)
        {
            result += (i * 17) ^ (i >> 3);
            result = (result * 13) + (i << 2);
            result ^= (result << 5) | (result >> 7);
        }
    }

private:
    uint64_t last_time_us{0};
    uint32_t count{0};
    uint64_t max_diff{0};
    uint64_t min_diff{UINT64_MAX};
    uint64_t sum_diff{0};
};
