#pragma once

#include <platform/mutex.hpp>
#include <platform/atomic.h>
#include <platform/log.h>



class hrt
{
public:
    hrt()
    {
        f_mutex::init(&hrt_mutex);
    }

    ~hrt()
    {
        stop();
    }

    bool set_alarm(uint32_t delay_us);
    bool set_alarm_at(uint64_t absolute_us);
    void set_alarm_at_for_isr(uint32_t absolute_ticks, uint32_t current_ticks);
    bool cancel_alarm();

    bool init(uint8_t channel = 0);
    int stop();
    int start();

    uint64_t get_current_us();
    uint64_t get_current_ns();
    uint32_t get_current_ticks();
    uint64_t get_current_ticks_64();

#if defined(FML_ZEPHYR)
    uint64_t ticks_to_us(uint32_t ticks);
    uint32_t us_to_ticks(uint64_t us);
#endif
    const void* get_device() const { return dev.z_dev; }

    bool is_available() const
    {
        return device_ok.load();
    }

protected:
    virtual void func_for_expiry() {}

#if defined(FML_ZEPHYR)
    friend void on_expiry(const struct device *dev, uint8_t chan_id,
                          uint32_t ticks, void *user_data);
#endif

    bool check_hrt();

private:
    fml::atomic_bool device_ok{false};

    mutex_t hrt_mutex;

    static const struct device *counter_dev;

    struct
    {
        void *z_dev = nullptr;   
        uint8_t channel_id = 0;
        uint32_t freq = 0;
    } dev;

    struct
    {
        void *callback_ptr = nullptr; // 预存回调指针
        void *user_data = nullptr;    // 预存 this 指针
    } alarm_cache;
};

void printf_hrt_info(hrt *timer);


#if defined(FML_ZEPHYR)
#include <zephyr/drivers/counter.h>

inline uint64_t hrt::ticks_to_us(uint32_t ticks)
{
    if (!dev.z_dev) {
        return 0;
    }
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    return counter_ticks_to_us(counter_dev, ticks);
}

inline uint32_t hrt::us_to_ticks(uint64_t us)
{
    if (!dev.z_dev) {
        return 0;
    }
    auto *counter_dev = static_cast<const struct device *>(dev.z_dev);
    return counter_us_to_ticks(counter_dev, us);
}
#endif