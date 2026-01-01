#include "workqueue_manager.hpp"
#include <platform/time.h>
#include <platform/common/atomic.hpp>
#include <drivers/hrtimer/hrt.hpp>
#include <lib/perf/perf.hpp>
#include <platform/log.h>
#include <lib/containers/heap.hpp>

typedef struct moment_t
{
    uint32_t moment;
    uint32_t cycle : 24;    // 24位：最大16,777,215 ticks (约200ms @ 84MHz)
    uint32_t map_index : 5; // 5位：支持32个任务
    uint32_t reserved : 3;  // 3位：保留

    moment_t(uint32_t moment, uint32_t cycle, uint8_t map_index)
        : moment(moment), cycle(cycle), map_index(map_index), reserved(0)
    {
        if (cycle > 16777215) // 2^24 - 1
        {
            fml_warn("moment_t: cycle %u exceeds 24-bit limit, truncated to %u\n",
                     cycle, this->cycle);
        }
    }

} moment_t; // 8B

extern fml::atomic<uint32_t> wq_map;

f_event_t wq_event;
// static uint32_t now_ticks;
uint32_t now_ticks;
moment_t *moments[32];
event_bits_t wq_ev_bits;
static int heap_size = 0;

// 为每个工作项记录触发时间
uint32_t isr_trigger_ticks[32] = {0};      // ISR读取的时间
uint32_t scheduled_moment_ticks[32] = {0}; // 当前预期触发时间
uint32_t last_scheduled_ticks[32] = {0};   // 上次预期触发时间（用于计算周期）
uint32_t isr_fire_count[32] = {0};         // ISR触发次数（调试用）

WorkQueue *WorkQueueManager::queues_mem[CONFIG_FML_MAX_PRIORITY] = {nullptr};

// ============ 环形时间比较器 ============

/**
 * @brief 环形时间比较器（用于最小堆）
 * @note 利用有符号整数溢出特性处理 32位 counter 溢出
 *       返回 true 表示 a 在时间上早于 b（a 更小，更优先）
 */
struct MomentCompare
{
    bool operator()(moment_t *const &a, moment_t *const &b) const
    {
        return static_cast<int32_t>(a->moment - b->moment) < 0;
    }
};

static constexpr MomentCompare moment_cmp{};

bool WorkQueueManager::hrt_table_init()
{
    fml_event_init(&wq_event);

    uint8_t index = 0;

    uint32_t current_ticks = WorkQueueManager::get_hrt().get_current_ticks();

    if (queues_mem[0] != nullptr)
    {
        for (int i = 0; i < queues_mem[0]->get_mem_count(); i++)
        {
            WorkItem *item = queues_mem[0]->get_item(i);
            uint32_t cycle_us = item ? item->cycle_in_100us() * 100 : 0;
            uint32_t cycle_ticks = WorkQueueManager::get_hrt().us_to_ticks(cycle_us);

            uint32_t first_moment;
            if (cycle_ticks == 0)
            {
                first_moment = current_ticks + WorkQueueManager::get_hrt().us_to_ticks(1000);
            }
            else
            {
                first_moment = current_ticks + cycle_ticks;
            }

            moments[index++] = new moment_t(first_moment, cycle_ticks, i);
        }
        queues_mem[0]->wq_start();
    }

    if (queues_mem[1] != nullptr)
    {
        for (int j = 0; j < queues_mem[1]->get_mem_count(); j++)
        {
            WorkItem *item = queues_mem[1]->get_item(j);
            uint32_t cycle_us = item ? item->cycle_in_100us() * 100 : 0;
            uint32_t cycle_ticks = WorkQueueManager::get_hrt().us_to_ticks(cycle_us);

            uint32_t first_moment;
            if (cycle_ticks == 0)
            {
                first_moment = current_ticks + WorkQueueManager::get_hrt().us_to_ticks(2000);
            }
            else
            {
                first_moment = current_ticks + cycle_ticks;
            }

            moments[index++] = new moment_t(first_moment, cycle_ticks, j + 16);
        }
        queues_mem[1]->wq_start();
    }

    if (index > 0)
    {
        heap_size = index;

        fml::MinHeap<moment_t *>::heapify(moments, heap_size, moment_cmp);

        WorkQueueManager::get_hrt().set_alarm_at_for_isr(moments[0]->moment, current_ticks);
        return true;
    }
    return false;
}

bool WorkQueueManager::normal_wq_init()
{
    bool all_success = true;

    for (int priority = 2; priority < CONFIG_FML_MAX_PRIORITY; priority++)
    {
        if (queues_mem[priority] == nullptr)
        {
            continue;
        }

        queues_mem[priority]->wq_start();

        for (int i = 0; i < queues_mem[priority]->get_mem_count(); i++)
        {
            WorkItem *item = queues_mem[priority]->get_item(i);

            if (!item || item->cycle_in_100us() == 0)
            {
                continue;
            }

            if (!item->init_and_schedule(&queues_mem[priority]->workqueue))
            {
                fml_warn("任务调度失败: priority=%d, task=%d\n", priority, i);
                all_success = false;
            }
        }
    }

    return all_success;
}

// ============ ISR 定时器回调 ============

void WorkQueueManager::expiry_call(void *arg)
{
    now_ticks = WorkQueueManager::get_hrt().get_current_ticks();
    uint32_t local_map = 0;

    while (heap_size > 0 && static_cast<int32_t>(moments[0]->moment - now_ticks) <= 0)
    {
        moment_t *m = fml::MinHeap<moment_t *>::pop(moments, heap_size, moment_cmp);

        local_map |= (1UL << m->map_index);

        if (m->cycle > 0)
        {
            m->moment += m->cycle;
            fml::MinHeap<moment_t *>::push(moments, heap_size, m, 32, moment_cmp);
        }
    }

    wq_map.fetch_or(local_map, __ATOMIC_RELEASE);

    if (heap_size > 0)
    {
        WorkQueueManager::get_hrt().set_alarm_at_for_isr(moments[0]->moment, now_ticks);
    }

    wq_ev_bits = (!!((uint16_t)local_map)) | (!!((local_map >> 16)) << 1);
    fml_event_post(&wq_event, wq_ev_bits);
}

// ============ 队列管理 ============

void WorkQueueManager::join_work_queue(WorkItem *work)
{
    uint8_t priority = work->get_priority();

    if (priority >= CONFIG_FML_MAX_PRIORITY)
    {
        fml_warn("Priority out of range\n");
        return;
    }

    if (!queues_mem[priority])
    {
        if (instance().workqueue_create(priority) == false)
        {
            fml_error("Work queue create failed\n");
            return;
        }
    }

    if ((work->work.opt & Allow_downgrade) &&
        queues_mem[priority]->get_mem_count() == WORKQUEUE_MAX_SIZE)
    {

        uint8_t next_priority = priority + 1;

        if (next_priority >= CONFIG_FML_MAX_PRIORITY)
        {
            fml_error("Cannot downgrade priority , already at minimum\n");
            return;
        }

        if (!queues_mem[next_priority])
        {
            if (instance().workqueue_create(next_priority) == false)
            {
                fml_error("Work queue create failed for downgraded priority\n");
                return;
            }
        }

        if (queues_mem[next_priority]->add_workitem(work) == false)
        {
            fml_error("queue members full\n");
        }
    }
    else
    {
        if (queues_mem[priority]->add_workitem(work) == false)
        {
            fml_warn("queue members full\n");
        }
    }
}

bool WorkQueueManager::workqueue_create(uint8_t priority)
{
    uint8_t prio = fml_priority_convert(priority);
    queues_mem[prio] = new WorkQueue(prio);

    queues_mem[prio]->tid = get_tid();
    return (queues_mem[prio] != nullptr);
}