#include "hrtcall.hpp"

void hrtcall::call_at(abstime time, void *arg)
{
    call_arg.store(arg);
    period.store(0); // 单次调用

    const uint64_t now = get_current_us();
    if (int64_t duration = time - now; duration > 0)
    {
        set_alarm(duration);
        is_scheduled.store(true);
    }
}

void hrtcall::call_after(time_t delay, void *arg)
{
    call_arg.store(arg);
    period.store(0); // 单次调用
    set_alarm(delay);
    is_scheduled.store(true);
}

void hrtcall::call_loop(time_t _period, void *arg)
{
    call_arg.store(arg);
    period.store(_period);

    // 计算第一次触发的绝对时间（在用户线程中，不在ISR）
    next_call_time = get_current_us() + _period;

    set_alarm(_period);
    is_scheduled.store(true);
}

void hrtcall::call_cancel()
{
    cancel_alarm();

    void *arg = call_arg.load();
    if (arg)
        stop_call(arg); // 调用用户清理函数

    is_scheduled.store(false);
    period.store(0);
}

void hrtcall::func_for_expiry()
{
    void *arg = call_arg.load();
    time_t period_val = period.load();

    expiry_call(arg);

    if (period_val > 0)
    {
        // 周期调用：使用绝对时间避免累积误差
        next_call_time += period_val;
        set_alarm_at(next_call_time);
    }
    else
    {
        is_scheduled.store(false);
    }
}