
#pragma once

/*
    Header file

*/
#include <platform/task.h>
#include <platform/define.h>
#include <platform/common/atomic.hpp>
#include <platform/log.h>
#include <platform/mutex.hpp>
#include "thread.h"
#include <lib/type_traits/type_traits.hpp>

extern mutex_t app_mutex;

#define app_lock mutex_lock
#define app_unlock mutex_unlock // todo : implement header file of thread mutex

/*

  static int task_spawn()
  {
    int ret = 0;
    ret = thread_create(&tid, NULL,
                                &run_alone, object);

  }

 */
enum class work_queue_level
{
    none,
    high, // 3-5
    mid,  // 7-9
    low   // 11-13
};

#define FML_REGISTER_APP(app_name)                         \
                                                           \
    extern "C" int app_name##_main(int argc, char *argv[]) \
    {                                                      \
        return app_name::main(argc, argv);                 \
    }

#ifdef __cplusplus

class WorkItem;

namespace fml
{
    /**
     * When use the workqueue, workitem should be constructed .
     *
     */
    template <class T>
    class AppBase
    {
    public:
        AppBase();

        AppBase(const char *name, size_t stack_size = 0)
        {
            task.task_name = name;
            // priority会在派生类中设置
            task.stack_size = stack_size;
        }

        virtual ~AppBase() = default; // why the Destructor with virtual

        static int main(int argc, char *argv[])
        {
            int ret = 0;
            if (strcmp(argv[1], "start") == 0)
            {
                ret = start();
            }
            else if (strcmp(argv[1], "stop") == 0)
            {
                ret = stop_and_cleanup();
            }
            else if (strcmp(argv[1], "status") == 0)
            {
                check_status();
            }
            else if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)
            {
                T::printf_usage_static();
            }

            app_lock(&app_mutex);
            // int ret = T::custom_commond(argc, argv);
            app_unlock(&app_mutex);

            return ret;
        }

        static T *instantiate()
        {
            app_lock(&app_mutex);

            T *_object = new T();
            if (_object)
            {
                int id = 0;

                if constexpr (fml::has_member_init_v<T>)
                {
                    _object->init();
                }

                if constexpr (fml::is_base_of_v<WorkItem, T>)
                {
                    id = workqueue_id(_object->get_priority());
                }
                else
                {
                    id = T::task_spawn();
                }

                if (id > 0)
                {
                    object.store(_object);
                    tid.store(id);
                }
                else
                {
                    delete _object;
                    _object = nullptr;
                }
            }

            app_unlock(&app_mutex);
            return _object;
        };

        // T::task_spawn();

        static int start()
        {
            app_lock(&app_mutex);

            if (is_running())
            {
                fml_info("task is running");
                app_unlock(&app_mutex);
                return 0;
            }

            app_unlock(&app_mutex);

            if (T::instantiate() == nullptr)
            {
                fml_error("task spawn failed");
                return -1; // 失败返回 -1
            }
            return 0; // 成功返回 0
        }

        static void check_status()
        {
            app_lock(&app_mutex);

            if (object.load() && is_running())
            {
                fml_info("task is running");
            }
            else
            {
                fml_info("task is not running");
            }

            app_unlock(&app_mutex);
        }

        static bool is_running() // true is running
        {
            return tid.load();
        }

        // static int stop()
        // {
        //     int ret = 0;

        //     app_lock(&app_mutex);

        //     if (is_running() && object.load())
        //     {
        //         is_exit.store(true);
        //         tid = 0;
        //     }

        // }

        virtual int printf_usage()
        {
            fml_info("usage : app [start|stop|status]"); //@todo : info printf
            return 0;
        }

        static int printf_usage_static()
        {
            fml_info("usage : app [start|stop|status]"); //@todo : info printf
            return 0;
        }

        static void run_alone(void *arg1, void *arg2, void *arg3)
        {
            T *_object = object.load();
            int ret = 0;

            if (_object && is_running())
            {
                _object->run();
            }
            else
            {
                fml_error("object is null");
                ret = -1;
            }

            stop_and_cleanup();
            // return ret;
        }

        /*
         *@brief: a main loop of application
         */
        virtual void run() {}

        int get_tid() const { return tid.load(); }

    protected:
        static int stop_and_cleanup()
        {
            int ret = 0;
            app_lock(&app_mutex);

            if (is_running() && object.load())
            {
                T *_object = object.load();
                tid.store(0);
                delete _object;
                object.store(nullptr);
                ret = 0;
            }
            else
            {
                ret = -1;
            }

            app_unlock(&app_mutex);
            return ret;
        }

        /* Used by run() to determine whether to exit */
        void check_or_exit()
        {
            if (request_exit())
            {
                if (!stop_and_cleanup())
                    fml_info("The task has been exited");
            }
        }

        bool request_exit()
        {
            return is_exit.load();
        }

        static f_thread_t task;

        static atomic<T *> object;

        /* The ID number of the thread it belongs to */
        static atomic_int tid;

    private:
        __attribute__((noinline)) void startup_info()
        {
            const char *func = __PRETTY_FUNCTION__;
            const char *start = strchr(func, '<') + 1;
            const char *end = strchr(start, '>');
            fml_info("[Module] startup : %.*s\n ", (int)(end - start), start); //@todo : info printf
        }

        atomic_bool is_exit{false};
    };
};

// 在全局命名空间提供 AppBase 别名
template <typename T>
using AppBase = fml::AppBase<T>;

template <class T>
fml::atomic<T *> fml::AppBase<T>::object{nullptr};

template <class T>
fml::atomic_int fml::AppBase<T>::tid{0};

template <class T>
f_thread_t fml::AppBase<T>::task{};

#endif /* __cplusplus */

/**
 * profile
 * two method of scheduling : work_queue(shared the thread) and independent thread
 *
 *
 *
 *
 *
 */
