#pragma once

#include <drivers/hrtimer/hrtcall.hpp>
#include <platform/apps.h>
#include <platform/log.h>

struct TestData
{
    uint64_t alarm_start_time;
    uint32_t expected_delay;
    int callback_count;
};

// 自定义定时器类，继承 hrtcall
class MyHrtCall : public hrtcall
{
public:
    MyHrtCall() : test_data{0, 0, 0} {}

    // 测试单次延迟调用
    void test_call_after(uint32_t delay_us)
    {
        test_data.expected_delay = delay_us;
        test_data.alarm_start_time = get_current_us();
        test_data.callback_count = 0;
        call_after(delay_us, &test_data);
    }

    // 测试周期调用
    void test_call_loop(uint32_t period_us)
    {
        test_data.expected_delay = period_us;
        test_data.alarm_start_time = get_current_us();
        test_data.callback_count = 0;
        call_loop(period_us, &test_data);
    }

protected:
    void expiry_call(void *arg) override
    {
        TestData *data = (TestData *)arg;
        if (data)
        {
            uint64_t trigger_time = get_current_us();
            uint64_t actual_delay;

            if (data->callback_count == 0)
            {
                actual_delay = trigger_time - data->alarm_start_time;
            }
            else
            {
                actual_delay = trigger_time - data->alarm_start_time - data->expected_delay * data->callback_count;
            }

            int64_t error = (int64_t)actual_delay - (int64_t)data->expected_delay;

            fml_info("[Callback #%d] Delay: %llu us, Error: %lld us\n",
                     data->callback_count, actual_delay, error);

            data->callback_count++;
        }
    }

    void stop_call(void *arg) override
    {
        fml_info("[Stop] Timer cancelled\n");
    }

private:
    TestData test_data;
};

class hrt_test : public fml::AppBase<hrt_test>
{
public:
    hrt_test() : fml::AppBase<hrt_test>("hrt_test", work_queue, WORK_QUEUE_HIGH_PRIORITY) {}
    ~hrt_test();

    void run() override;

private:
    MyHrtCall timer;
};
