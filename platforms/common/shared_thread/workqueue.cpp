#include "workqueue.hpp"
#include <platform/event.h>
#include <platform/common/atomic.hpp>

/**
 * high 16 bit for workqueue 1, low 16 bit for workqueue 0
 * 使用原子变量保证ISR和线程间的线程安全
 */
fml::atomic<uint32_t> wq_map{0};

extern f_event_t wq_event;

void workqueue_thread_entry(void *p1, void *p2, void *p3)
{
    WorkQueue *wq = static_cast<WorkQueue *>(p1);
    if (wq)
    {
        wq->workqueue_run();
    }
}

void WorkQueue::wq_start()
{
    // 调用平台抽象层的队列启动接口
    wqset_t cfg;
    cfg.stack = nullptr; // 由平台层分配
    cfg.stack_size = 2048;
    cfg.prio = priority;
    cfg.thread_entry = workqueue_thread_entry; // 设置线程入口
    cfg.user_data = this;                      // 传递 WorkQueue 指针

    queue_set_and_start(&workqueue, &cfg);
}

bool WorkQueue::is_pending(WorkItem *item)
{
    // 检查位图中对应位是否被置位
    for (int i = 0; i < mem_count; i++)
    {
        if (this->item[i] == item)
        {
            uint32_t map = wq_map.load(__ATOMIC_ACQUIRE);
            uint8_t bit_index = priority ? (16 + i) : i;
            return (map & (1UL << bit_index)) != 0;
        }
    }
    return false;
}

void WorkQueue::workqueue_run()
{
    uint8_t start_bit = priority ? 16 : 0;
    event_bits_t my_event = (priority == 0) ? 0x01 : 0x02;

    while (1)
    {
        fml_event_wait_any(&wq_event, my_event);

        // 处理所有待执行的任务
        foreach_map(start_bit, 16);
    }
}

void WorkQueue::foreach_map(uint8_t start_bit, uint8_t scope)
{
    uint32_t bitmask = (((1U << scope) - 1) << start_bit);

    uint32_t current_map = wq_map.fetch_and(~bitmask, __ATOMIC_ACQ_REL);
    uint16_t temp_mask = (current_map & bitmask) >> start_bit;

    while (temp_mask)
    {
        item[__builtin_ctz(temp_mask)]->run();
        temp_mask &= (temp_mask - 1);
    }
}
