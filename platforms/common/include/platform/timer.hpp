#pragma once

#include <stdint.h>
#include "define.h"

BEGIN_DECLS

typedef uint64_t f_abstime_t;
typedef uint32_t f_time_t;

#if defined(FML_ZEPHYR)
#include <zephyr/kernel.h>
#define f_timer_t struct k_timer
typedef void (*hrt_callback_t)(void *);
typedef void (*hrt_func_t)(f_timer_t);

#elif defined(FML_LINUX)

#define f_timer_t int
#endif

END_DECLS

#ifdef __cplusplus

namespace fml
{
    class timer
    {
    public:
        timer()
        {
            timer_init(&timer);
            k_timer_init(&sleep_timer, NULL, NULL); // Initialize sleep timer without callbacks
        }

        ~timer()
        {
            timer_stop(&timer);
        }

        /**
         * @brief Callback when timer expire
         *
         * @param func
         */
        virtual void timer_for_expiry()
        {
        }

        /**
         * @brief Callback when timer is stopped
         *
         * @param func

         */
        virtual void timer_for_stop()
        {
        }

        /**
         * @brief Start timer
         * @param after：start the timer after this time
         * @param period：timer period , if period is 0, timer will only expire once
         */
        void hrt_start(f_time_t after, f_time_t period);

        /**
         * high resolution timer sleep in us
         */
        void hrt_sleep(f_time_t sleeptime);

        /**
         * sleep until a specific time
         * @param f_abstime_t
         */
        void hrt_sleep_until(f_abstime_t f_abstime_t);

        f_timer_t timer;

    protected:
        void timer_init(f_timer_t *timer);
        void timer_stop(f_timer_t *timer);

    private:
        f_timer_t sleep_timer; // Dedicated timer for sleep (no callbacks)
    };

    class hrtcall : public timer
    {
    public:
        hrtcall()
        {
        }

        ~hrtcall()
        {
        }

        hrtcall(const hrtcall &) = delete;
        hrtcall &operator=(const hrtcall &) = delete;

        void hrt_call_at(f_abstime_t time, hrt_callback_t callback, void *arg);

        void hrt_call_after(f_time_t time, hrt_callback_t callback, void *arg);

        void hrt_call_loop(f_time_t delaytime, hrt_callback_t callback, void *arg);

        void hrt_call_cancel();

        virtual void stop_call(void *arg) {};

        virtual void expiry_call(void *arg) {};

    private:
        hrt_callback_t callback;
        void *clbk_arg{nullptr};
    };
};

#endif // cplusplus