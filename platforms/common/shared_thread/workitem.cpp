#include "workitem.hpp"
#include "workqueue_manager.hpp"
#include <platform/log.h>
#include <platform/task.h>

WorkItem::WorkItem(opt_t _opt = none) : cycle(0)
{
    work.opt = _opt;
    priority = WORK_QUEUE_MID_PRIORITY;
    WorkQueueManager::join_work_queue(this);
}

WorkItem::WorkItem(uint8_t _priority, uint32_t _cycle) : priority(_priority)
{
    cycle = _cycle / 100;
    WorkQueueManager::join_work_queue(this);
}

void WorkItem::delayed_handler(f_work_t *_work)
{
    f_work_delayable_t *dwork = CONTAINER_OF(_work, f_work_delayable_t, work);
    fwork_t *wk = CONTAINER_OF(dwork, fwork_t, work_delay);

    WorkItem *workitem = CONTAINER_OF(wk, WorkItem, work);
    workitem->run();

    if (workitem->cycle_in_100us() > 0)
    {
        f_wq_t *queue = _work->queue;
        uint32_t delay_us = workitem->cycle_in_100us() * 100;

#ifdef FML_ZEPHYR
        workitem_reschedule(&wk->work_delay, queue, delay_us) #endif
    }
}

void WorkItem::event_handler(f_work_t *_work)
{
}