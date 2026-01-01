#include "workitem.hpp"

class SchedWorkItem : public WorkItem
{
public:
    SchedWorkItem(uint8_t _priority) : WorkItem(_priority)
    {
    }

    void scheduleNow();

    void scheduleAfter();

    void scheduleInterval();

    ~SchedWorkItem();
};