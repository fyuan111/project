#pragma once

#include <platform/queue.h>
#include <platform/define.h>
#include <platform/log.h>

class WorkItem
{
public:
    /* For single work item */
    WorkItem(opt_t opt);
    WorkItem(uint8_t _priority, uint32_t _cycle);

    virtual ~WorkItem() = default;

    void init_work()
    {
        if (priority < 2)
            return;

        if (cycle > 0)
        {
            work.opt = SUBMIT_DELAY;
            workitem_delayable_init(&work, delayed_handler);
        }
        else
        {
            work.opt = SUBMIT_EVENT;
            workitem_init(&work, event_handler);
        }
    }

    bool init_and_schedule(f_wq_t *queue)
    {
        if (priority >= 2)
        {
            if (cycle > 0)
            {
                work.opt |= Periodic;
            }
            else if (work.opt & Alarm)
            {
            }

            workitem_delayable_init(&work, delayed_handler);

            uint32_t delay_us = cycle * 100;
            int ret = workitem_reschedule(&work.work_delay, queue, delay_us);

            return ret >= 0;
        }
        return false;
    }

    uint8_t get_priority() const { return priority; }
    uint8_t cycle_in_100us() const { return cycle; }

    static void delayed_handler(f_work_t *_work);
    static void event_handler(f_work_t *_work);

protected:
    friend class WorkQueue;
    friend class WorkQueueManager;

    /**
     * @brief 用户实现的任务函数
     */
    virtual void run() = 0;

    uint8_t cycle{0};

private:
    fwork_t work;
    uint8_t priority{0};
};
