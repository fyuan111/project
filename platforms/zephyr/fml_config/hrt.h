// 定义平台相关的定时器节点
#if defined(CONFIG_SOC_SERIES_ESP32S3)
#define time_tR_NODE timer0
#elif defined(CONFIG_SOC_SERIES_STM32F4X)
#define time_tR_NODE timers2 // TIM2: 32位, APB1
#else
#error "Unsupported platform for HRT"
#endif