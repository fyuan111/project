#include <include/platform/common/thread.h>
#include <platform/task.h>
#include <shared_thread/workqueue_manager.hpp>

fml::atomic_int globe_tid{1};

int get_tid()
{
    return globe_tid.fetch_add(1);
}


uint8_t workqueue_id(uint8_t priority)
{
    int prio = fml_priority_convert(priority);
    WorkQueue* queue = WorkQueueManager::get_queue(prio);
    return (queue == nullptr ? 0 : queue->get_tid());
}