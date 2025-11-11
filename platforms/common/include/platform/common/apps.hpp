
#pragma once

/*
    Header file

*/
#include <platform/atomic.hpp>

extern thread_mutex_t app_mutex;

#define app_lock thread_mutex_lock
#define app_unlock thread_mutex_unlock // todo : implement header file of thread mutex

#ifdef __cplusplus

namespace fml
{
    template <class T>
    class AppBase
    {
    public:
        app() 
        {
#if defined(DEBUG)

            startup_info();
#endif
        }

        virtual ~app() = default; // why the Destructor with virtual

        int main(int argc, char *argv[])
        {
            if(strcmp(argv[1] , "start"))
            {
                return start();
            }
            else if(strcmp(argv[1] , "stop"))
            {
                return stop();
            }
            else if(strcmp(argv[1] , "status"))
            {
                return status();
            }
            else if(strcmp(argv[1] , "help") || strcmp(argv[1] , "-h"))
            {
                return T::printf_usage();
            }

            app_lock(&app_mutex);
            int ret = T::custom_commond(argc, argv);
            app_unlock(&app_mutex);

            return ret;
        }

        virtual void init() {};

        static void start()
        {
            int ret = 0;

            app_lock(&app_mutex);

            if (is_running())
            {
                fml_info("task is running");
            }
            else
            {
                ret = fml_spawn(); // @todo : spawn
                if (ret < 0)
                {
                    fml_error("task spawn faided") // @todo : info printf
                }
            }
            app_unlock(&app_mutex);
            return ret;
        }

        static void stop()
        {
            task_id = -1;

        }

        static void stop_and_cleanup()
        {

        }

        static int status()
        {
            
        }

        bool is_running() // true is running
        {
            return task_id != 0;
        }

        /*
         *@brief: a main loop of
         */
        virtual void run() {}

        virtual int printf_usage()
        {
            fml_info("usage : app [start|stop|status]"); //@todo : info printf
            return 0;
        }

        static atomic<T *> object;

        static atomic_int task_id;

    private:

        __attribute__((noinline)) void startup_info()
        {
            const char* func = __PRETTY_FUNCTION__;
            const char* start = strchr(func, '<') + 1;
            const char* end = strchr(start, '>');
            fml_info("[Module] startup : %.*s\n ",(int)(end - start), start); //@todo : info printf
        }

        atomic_bool is_exit{false};
    };

};

template <class T>
fml::atomic<T *> AppBase<T>::object = nullptr;

template <class T>
fml::atomic_int AppBase<T>::task_id = 0;

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
