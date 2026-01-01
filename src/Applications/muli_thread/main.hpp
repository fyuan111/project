#include <platform/apps.h>
#include <platform/atomic.h>
#include <perf/perf.hpp>
#include <idc/publication.hpp>
#include <msg/sensor.h>

class dy_thread : public AppBase<dy_thread, wq_none>
{
public:
    dy_thread() : pub(msg(sensor)) {};
    ~dy_thread() = default;

    void run() override;

    static int task_spawn();

private:
    Perf counter; // 成员变量，保存统计信息
    uint32_t loop_count{0};

    Publisher<sensor_s> pub;
};
