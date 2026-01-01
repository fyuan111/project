#pragma once

#include <stdint.h>
#include <drivers/hrtimer/hrtcall.hpp>
#include <platform/sem.h>
#include <platform/queue.h>
#include <platform/task.h>

#include "workitem.hpp"

using namespace fml;

#define WORKQUEUE_MAX_SIZE 16

// 前向声明原子类型
namespace fml
{
    template <typename T>
    class atomic;
}
extern fml::atomic<uint32_t> wq_map;

class WorkQueue
{
    // 声明友元函数
    friend void workqueue_thread_entry(void *p1, void *p2, void *p3);
    friend class WorkQueueManager;

public:
    WorkQueue() = delete;

    WorkQueue(uint8_t priority) : priority(priority)
    {
        queue_init(&workqueue);
        // wq_start() 由 hrt_table_init() 统一调用
    }

    virtual ~WorkQueue()
    {
        for (int i = 0; i < mem_count; i++)
            delete item[i];
    };

    /**
     * @brief 启动工作队列处理线程
     */
    void wq_start();

    /**
     * @brief 检查任务是否待执行
     */
    bool is_pending(WorkItem *item);

    uint8_t runtime_count()
    {
        return runing_count;
    }

    uint8_t get_mem_count() const { return mem_count; }

    uint8_t get_tid() const { return tid; }

    WorkItem *get_item(uint8_t index) const
    {
        return (index < mem_count) ? item[index] : nullptr;
    }

    bool add_workitem(WorkItem *workitem)
    {
        if (mem_count < WORKQUEUE_MAX_SIZE)
        {
            item[mem_count++] = workitem;
            return true;
        }
        return false;
    }

    /**
     * @brief 等待工作项数量稳定
     * @return 当前工作项数量
     */
    uint8_t wait_workitem_stable()
    {
        uint8_t prev_count = 0;
        for (int i = 0; i < 10; i++)
        {
            if (mem_count > prev_count)
            {
                prev_count = mem_count;
                f_msleep(10);
            }
        }
        return mem_count;
    }

protected:
    void workqueue_run();

private:
    void foreach_map(uint8_t start_bit, uint8_t scope);

    uint8_t priority;
    uint8_t tid;
    uint8_t mem_count{0};
    uint8_t runing_count{0};
    f_wq_t workqueue;
    WorkItem *item[WORKQUEUE_MAX_SIZE];
};
