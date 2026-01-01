#include <zephyr/kernel.h>
#include <platform/time.h>
#include <platform/task.h>
#include <platform/common/atomic.hpp>
#include <platform/log.h>
#include <msg/topic_registry.h>
#include <shared_thread/workqueue_manager.hpp>

extern "C" int builtin_command_execute(const char *name, int argc, char *argv[]);
extern "C" void builtin_command_list(void);
extern "C" int builtin_command_count(void);
extern "C" const char *builtin_command_get_name(int index);

int main(int argc, char **argv)
{
    // 启动阶段：提升主线程优先级
    k_thread_priority_set(k_current_get(), K_HIGHEST_APPLICATION_THREAD_PRIO);

    // FML 启动 Banner
    printk("\n");
    printk("  ______     __     __      __       \n");
    printk(" /_____/|   /  |   |  \\    /_/|      \n");
    printk("|  ____|/  |   \\   /   |   | ||      \n");
    printk("| |__/|    |    \\ /    |   | ||           \n");
    printk("|  __|/    |  |\\ _ /|  |   | ||____   \n");
    printk("| ||       |  |     |  |   | |/___/|  \n");
    printk("|_|/       |__|     |__|   |______|/  \n");
    printk("\n");
    printk("           >> F M L << \n");
    printk("    ===========================\n");
    printk("    Platform: Zephyr RTOS\n");
    printk("    Build: %s %s\n", __DATE__, __TIME__);
    printk("========================================\n");

    // 列出所有可用命令
    builtin_command_list();
    printk("\n");
    printk(" ========================= START INITIALIZING\n");
    // 初始化 IDC 话题（必须在启动应用之前）
    idc_register_all_topics();

    // 自动启动所有注册的应用
    int app_count = builtin_command_count();

    for (int i = 0; i < app_count; i++)
    {
        const char *app_name = builtin_command_get_name(i);
        if (app_name)
        {
            // printk("\n[APP] Starting: %s\n", app_name);

            // 构造 argc/argv: "app_name start"
            char *start_argv[] = {(char *)app_name, (char *)"start"};
            int ret = builtin_command_execute(app_name, 2, start_argv);

            if (ret == 0)
            {
                printk("[OK] %s\n", app_name);
            }
            else
            {
                printk("[FAIL] %s (ret=%d)\n", app_name, ret);
            }
        }
    }
    printk(" ========================= INITIALIZATION COMPLETED \n\n");

    // 初始化工作队列管理器和定时器调度
    WorkQueueManager::instance().wq_manager_init();

    k_thread_priority_set(k_current_get(), 10);

    while (1)
    {
        f_sleep(K_SECONDS(2));
    }
    return 0;
}