
#pragma once

/*
    Header file

*/
#include <platform/atomic.hpp>
#include <platform/log.h>

extern thread_mutex_t app_mutex;

#define app_lock thread_mutex_lock
#define app_unlock thread_mutex_unlock // todo : implement header file of thread mutex


/*

  static int task_spawn()
  {
    int ret = 0;
    ret = thread_create(&task_id, NULL, 
                                &run_alone, object); 
    
  }

 */
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
    template <class T , work_queue_level _level = work_queue_level::mid>
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
                return check_status();
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

        static T* instantiate()
        {
            app_lock(&app_mutex);

            int id = 0;
            T * _object = new T();
            if(_object)
            {
                if(level != work_queue_level::none)
                {
                    /* @todo : join work queue */
                    /* if success, return the thread id , otherwise return -1 */
                    id = join_work_queue(_object, level);    
                }
                else if (level == work_queue_level::none)
                {
                    id = T::task_spawn(_object);
                }

                if (id > 0)
                {
                    object.store(_object);
                    task_id.store(id);
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
                app_unlock(&app_mutex);
                T* object = T::instantiate(); // @todo : spawn
                if (object == nullptr)
                {
                    fml_error("task spawn faided ,can see template param ") // @todo : info printf
                }
                return ret;
            }
            
            app_unlock(&app_mutex);
            return ret;
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

        static int run_alone(void* arg)
        {
            T* _object = object.load();
            int ret = 0;

            if(_object && is_running())
            {
                _object->run();
            }
            else
            {
                fml_error("object is null");
                ret = -1;
            }

            stop_and_cleanup();
            return ret;
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
                T* _object = object.load();
                task_id = 0;
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
        static void check_or_exit()
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

template <typename T , work_queue_level _level>
using AppBase = fml::AppBase<T , _level>;

template <class T , work_queue_level _level>
fml::atomic<T *> AppBase<T ,_level>::object{nullptr};

template <class T , work_queue_level _level>
fml::atomic_int AppBase<T , _level>::task_id{0};


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
