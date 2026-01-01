#include "main.hpp"
#include <platform/task.h>
#include <zephyr/kernel.h>
#include <platform/time.h>
#include <perf/perf.hpp>

K_THREAD_STACK_DEFINE(thread1_stack, 1024);

int dy_thread::task_spawn()
{
    static struct k_thread my_thread_data1;

    int id1 = fml::globe_tid.fetch_add(1);

    // 先设置 custom_data
    my_thread_data1.custom_data = (void *)(intptr_t)id1;

    // 然后创建线程（优先级改为正数）
    k_tid_t tid1 = k_thread_create(&my_thread_data1,
                                   thread1_stack,
                                   K_THREAD_STACK_SIZEOF(thread1_stack),
                                   run_alone,
                                   NULL,
                                   NULL,
                                   NULL,
                                   THREAD_MID_PRIORITY,
                                   0,
                                   K_MSEC(100)); // 延迟100ms启动

    (void)tid1; // 避免未使用变量警告

    return id1;
}

void dy_thread::run()
{
    // 重置性能计数器
    counter.reset();

    if (!pub.valid())
    {
        fml_error("Publisher failed to create!\n");
        return;
    }

    fml_info("Publisher started\n");

    sensor_s data = {0};

    while (1)
    {
        counter.start();

        // 更新数据
        data.timestamp = f_cycle_get_32();
        data.x += 0.1f;
        data.y += 0.2f;
        data.z += 0.3f;

        // 发布
        if (pub.publish(data))
        {
            // fml_info("Published: x=%.1f, y=%.1f, z=%.1f\n", data.x, data.y, data.z);
        }
        else
        {
            fml_error("Publish failed!\n");
        }

        k_usleep(1000000);

        counter.stop();
        // counter.print_runtime();
    }
}

FML_REGISTER_APP(dy_thread)
