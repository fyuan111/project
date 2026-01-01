#include <platform/apps.h>
#include <platform/log.h>
#include <platform/task.h>
#include <platform/common/atomic.hpp>
#include <platform/time.h>
#include <idc/subscription.hpp>
#include <msg/sensor.h>

class hello : public AppBase<hello>
{
public:
    hello() : sub(msg(sensor)),AppBase("hello",)
    {

        fml_info("hello world\n");
    }

    static int task_spawn();

    void run() override;
    ~hello()
    {
        fml_info("thanks\n");
    }

private:
    Subscriber<sensor_s> sub;
};

fml_task_define(hello_globe_tid, 1024, hello::run_alone, nullptr, nullptr, nullptr, THREAD_LOW_PRIORITY, 0, 1000);

int hello::task_spawn()
{
    // 从 globe_tid 获取并递增
    int hello_id = fml::globe_tid.fetch_add(1);

    // 为 hello_globe_tid 线程设置 custom_data
    hello_globe_tid->custom_data = (void *)(intptr_t)hello_id;

    return hello_id;
}

void hello::run()
{
    if (!sub.try_init())
    {
        fml_error("Subscriber failed - topic not found after retry!\n");
        return;
    }

    if (!sub.is_valid())
    {
        fml_error("Subscriber failed - topic not found!\n");
        return;
    }

    fml_info("Subscriber started, waiting for data...\n");

    sensor_s data = {0};

    while (1)
    {
        if (sub.update())
        {
            if (sub.copy(&data))
            {
                //     fml_info("  -> Received: x=%.1f, y=%.1f, z=%.1f\n",
                //              (double)data.x, (double)data.y, (double)data.z);
            }
        }
        f_sleep(K_MSEC(500));
    }
}

FML_REGISTER_APP(hello);
