#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <shared_thread/workqueue_manager.hpp>
#include <shared_thread/workqueue.hpp>
#include <drivers/hrtimer/hrtcall.hpp>

// 外部声明时间戳数组
extern uint64_t isr_timestamp_us[32];
extern uint64_t isr_end_timestamp_us[32];

// 测试统计数据
static volatile int test_run_count = 0;
static int64_t isr_duration_samples[100] = {0};
static int64_t schedule_latency_samples[100] = {0};
static int64_t total_latency_samples[100] = {0};

/**
 * @brief 延迟测试工作项
 */
class LatencyTestItem : public WorkItem
{
public:
    LatencyTestItem() : WorkItem(0, 10) {}  // Priority 0, 10ms 周期

    void run() override
    {
        if (test_run_count >= 100) {
            return;  // 采集够了
        }

        // 读取时间戳
        uint64_t run_time_us = WorkQueueManager::get_hrt().get_current_us();
        uint64_t isr_start_us = isr_timestamp_us[0];
        uint64_t isr_end_us = isr_end_timestamp_us[0];

        // 计算延迟
        int idx = test_run_count;
        isr_duration_samples[idx] = (int64_t)(isr_end_us - isr_start_us);
        schedule_latency_samples[idx] = (int64_t)(run_time_us - isr_end_us);
        total_latency_samples[idx] = (int64_t)(run_time_us - isr_start_us);

        test_run_count++;
    }
};

static LatencyTestItem *test_item = nullptr;

/**
 * @brief 测试前初始化
 */
static void *workqueue_latency_setup(void)
{
    printk("=== 工作队列延迟测试初始化 ===\n");

    // 注册测试任务
    test_item = new LatencyTestItem();
    WorkQueueManager::instance().register_item(test_item);

    // 初始化并启动调度
    WorkQueueManager::instance().hrt_table_init();

    return NULL;
}

/**
 * @brief 测试：基础调度延迟
 */
ZTEST(workqueue_latency, test_basic_latency)
{
    printk("\n=== 测试：基础调度延迟 ===\n");

    // 等待采集 100 个样本（10ms × 100 = 1秒）
    k_msleep(1500);

    // 确保采集完成
    zassert_equal(test_run_count, 100, "未采集足够样本: %d/100", test_run_count);

    // 计算统计数据
    int64_t isr_sum = 0, schedule_sum = 0, total_sum = 0;
    int64_t isr_min = INT64_MAX, schedule_min = INT64_MAX, total_min = INT64_MAX;
    int64_t isr_max = INT64_MIN, schedule_max = INT64_MIN, total_max = INT64_MIN;

    for (int i = 0; i < 100; i++) {
        isr_sum += isr_duration_samples[i];
        schedule_sum += schedule_latency_samples[i];
        total_sum += total_latency_samples[i];

        if (isr_duration_samples[i] < isr_min) isr_min = isr_duration_samples[i];
        if (isr_duration_samples[i] > isr_max) isr_max = isr_duration_samples[i];

        if (schedule_latency_samples[i] < schedule_min) schedule_min = schedule_latency_samples[i];
        if (schedule_latency_samples[i] > schedule_max) schedule_max = schedule_latency_samples[i];

        if (total_latency_samples[i] < total_min) total_min = total_latency_samples[i];
        if (total_latency_samples[i] > total_max) total_max = total_latency_samples[i];
    }

    int64_t isr_avg = isr_sum / 100;
    int64_t schedule_avg = schedule_sum / 100;
    int64_t total_avg = total_sum / 100;

    // 打印结果
    printk("\n=== 延迟测试结果 (样本数: 100) ===\n");
    printk("\nISR 执行时间:\n");
    printk("  平均: %lld us\n", isr_avg);
    printk("  最小: %lld us\n", isr_min);
    printk("  最大: %lld us\n", isr_max);
    printk("  抖动: %lld us\n", isr_max - isr_min);

    printk("\n线程调度延迟 (ISR结束 → 任务执行):\n");
    printk("  平均: %lld us\n", schedule_avg);
    printk("  最小: %lld us\n", schedule_min);
    printk("  最大: %lld us\n", schedule_max);
    printk("  抖动: %lld us\n", schedule_max - schedule_min);

    printk("\n总延迟 (ISR触发 → 任务执行):\n");
    printk("  平均: %lld us\n", total_avg);
    printk("  最小: %lld us\n", total_min);
    printk("  最大: %lld us\n", total_max);
    printk("  抖动: %lld us\n", total_max - total_min);

    // 性能断言（根据你的系统调整阈值）
    zassert_true(isr_avg < 5, "ISR执行时间过长: %lld us", isr_avg);
    zassert_true(schedule_avg < 15, "调度延迟过长: %lld us", schedule_avg);
    zassert_true(total_avg < 20, "总延迟过长: %lld us", total_avg);

    // 抖动断言
    zassert_true((isr_max - isr_min) < 5, "ISR抖动过大: %lld us", isr_max - isr_min);
    zassert_true((schedule_max - schedule_min) < 10, "调度抖动过大: %lld us",
                 schedule_max - schedule_min);

    printk("\n✅ 所有性能指标测试通过！\n\n");
}

// 测试套件定义
ZTEST_SUITE(workqueue_latency, NULL, workqueue_latency_setup, NULL, NULL, NULL);
