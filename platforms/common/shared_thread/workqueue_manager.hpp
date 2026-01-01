#pragma once

#include <platform/atomic.h>
#include <platform/mutex.hpp>
#include <platform/task.h>
#include <platform/apps.h>
#include <platform/event.h>
#include <drivers/hrtimer/hrtcall.hpp>
#include <platform/common/thread.h>
#include <platform/queue.h>
#include "workqueue.hpp"

#include <stdint.h>

// 前向声明
class WorkQueue;
class WorkItem;
extern f_event_t wq_event; // 全局事件组

/**
 * @brief 工作队列管理器（单例）
 *
 * 管理所有优先级的工作队列，按需创建
 */
class WorkQueueManager : public hrtcall
{
public:
    static WorkQueueManager &instance()
    {
        static WorkQueueManager instance;
        return instance;
    }

    /**
     * @brief 加入指定优先级的工作队列（懒加载）
     * @param priority 优先级（0 = 最低）
     */
    static void join_work_queue(WorkItem *work);

    /**
     * @brief 获取指定优先级的工作队列
     * @param priority 优先级
     * @return 成功返回工作队列指针，否则为 NULL
     */
    static WorkQueue *get_queue(uint8_t priority)
    {
        if (priority >= CONFIG_FML_MAX_PRIORITY)
        {
            fml_warn("Priority out of range\n");
            return nullptr;
        }

        return queues_mem[priority];
    }

    void wq_manager_init()
    {
        normal_wq_init();
        hrt_table_init();
    }

    bool normal_wq_init();
    bool hrt_table_init();

    static hrtcall &get_hrt()
    {
        return instance();
    }

private:
    friend uint8_t workqueue_id(uint8_t priority);

    void expiry_call(void *arg) override;

    WorkQueueManager()
    {
        f_mutex::init(&mgr_mutex);
    }

    ~WorkQueueManager()
    {
        for (int i = 0; i < CONFIG_FML_MAX_PRIORITY; i++)
        {
            if (queues_mem[i] != nullptr)
                delete queues_mem[i];
        }
    }

    // 禁止拷贝
    WorkQueueManager(const WorkQueueManager &) = delete;
    WorkQueueManager &operator=(const WorkQueueManager &) = delete;

    /**
     * @brief 创建指定优先级的工作队列
     */
    bool workqueue_create(uint8_t priority);

    static WorkQueue *queues_mem[CONFIG_FML_MAX_PRIORITY];

    mutex_t mgr_mutex; // TODO: 加锁保护 join_work_queue
};
