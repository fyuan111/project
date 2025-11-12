
#pragma once

/*
    Header file

*/
#include <platform/atomic.hpp>
#include <platform/log.h>

extern thread_mutex_t app_mutex;

#define app_lock thread_mutex_lock
#define app_unlock thread_mutex_unlock // todo : implement header file of thread mutex

enum class work_queue_level
{
    none ,
    high ,        // 3-5
    mid  ,        // 7-9
    low           // 11-13
};

#ifdef __cplusplus

namespace fml
{
    template <class T , work_queue_level _level = work_queue_level::none>
    class AppBase
    {
    public:

        AppBase()
        {
#if defined(DEBUG)

            startup_info();    
#endif
        }

        virtual ~AppBase() = default; // why the Destructor with virtual

        int main(int argc, char *argv[])
        {
            if (strcmp(argv[1], "start") == 0)
            {
                return start();
            }
            else if (strcmp(argv[1], "stop") == 0)
            {
                return stop();
            }
            else if (strcmp(argv[1], "status") == 0)
            {
                return status();
            }
            else if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)
            {
                return T::printf_usage();
            }

            app_lock(&app_mutex);
            int ret = T::custom_commond(argc, argv);
            app_unlock(&app_mutex);

            return ret;
        }

        virtual int instantiate()
        {
            int ret{0};

            app_lock(&app_mutex);
            if(level == work_queue_level::none)
            return -1;
            T *object = new T(level);
            if (object)
            {
                /* @todo : join work queue */
                /* if success, return the thread id , otherwise return -1 */
                ret = join_work_queue(object, level);
                if (ret > 0)
                {
                    this->object.store(object);
                    task_id.store(ret);
                }
                
                
            }

            app_unlock(&app_mutex);
            return ret;
        };

        static int start()
        {
            int ret = 0;

            app_lock(&app_mutex);

            if (is_running())
            {
                fml_info("task is running");
            }
            else
            {
                ret = instantiate(); // @todo : spawn
                if (ret < 0)
                {
                    fml_error("task spawn faided ,can see template param ") // @todo : info printf
                }
            }
            task_id.store();
            

            app_unlock(&app_mutex);
            return ret;
        }

        static void check_status()
        {
            app_lock(&app_mutex);

            if (_object.load() && is_running())
            {
                fml_info("task is running");
            }
            else
            {
                fml_info("task is not running");
            }

            app_unlock(&app_mutex);
        }

        bool is_running() // true is running
        {
            return task_id != 0;
        }

        // static int stop()
        // {
        //     int ret = 0;

        //     app_lock(&app_mutex);

        //     if (is_running() && object.load())
        //     {
        //         is_exit.store(true);
        //         task_id = 0;
        //     }

        // }

        virtual int printf_usage()
        {
            fml_info("usage : app [start|stop|status]"); //@todo : info printf
            return 0;
        }

        /*
         *@brief: a main loop of
         */
        virtual void run() {}

    protected:
        static int stop_and_cleanup()
        {
            int ret = 0;
            app_lock(&app_mutex);

            if (is_running() && object.load())
            {
                task_id = 0;
                delete object.load();
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
        static void check_or_exit()
        {
            if (request_exit())
            {
                if (stop_and_cleanup())
                    fml_info("The task has been exited");
            }
        }

        bool request_exit()
        {
            return is_exit.load();
        }

        static atomic<T *> object;

        static atomic_int task_id;

        static constexpr work_queue_level level = _level;
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

template <typename T>
using AppBase = fml::AppBase<T>;

template <class T>
fml::atomic<T *> AppBase<T>::object{nullptr};

template <class T>
fml::atomic_int AppBase<T>::task_id{0};


#endif /* __cplusplus */

/**
 * profile£º
 * two method of scheduling : work_queue(shared the thread) and independent thread
 *
 *
 *
 *
 *
 */
