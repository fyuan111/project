#pragma once 


#ifdef __ZEPHYR__
#include <zephyr/kernel.h>

#define printf printk
#define get_time()  k_uptime_get_32()

#elif defined(__linux__)

#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>

static inline uint32_t get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#else 
   #error "Unsupported platform"

#endif


#define FML_LOG_LEVEL_DEBUG  0
#define FML_LOG_LEVEL_INFO   1
#define FML_LOG_LEVEL_WARN   2
#define FML_LOG_LEVEL_ERROR  3

#ifndef FML_LOG_LEVEL
    #ifdef DEBUG
        #define FML_LOG_LEVEL FML_LOG_LEVEL_DEBUG
    #else
        #define FML_LOG_LEVEL FML_LOG_LEVEL_INFO
    #endif
#endif

#ifdef __linux__

#define FML_COLOR_RESET   "\033[0m"
#define FML_COLOR_DEBUG   "\033[36m"  // Cyan
#define FML_COLOR_INFO    "\033[32m"  // Green
#define FML_COLOR_WARN    "\033[33m"  // Yellow
#define FML_COLOR_ERROR   "\033[31m"  // Red

#else 

#define FML_COLOR_RESET   ""
#define FML_COLOR_DEBUG   ""
#define FML_COLOR_INFO    ""
#define FML_COLOR_WARN    ""
#define FML_COLOR_ERROR   ""

#endif  /*color enable*/


#ifndef FML_LOG_TIMESTAMP
    #define FML_LOG_TIMESTAMP 1
#endif

#ifndef FML_LOG_COLOR 
    #define FML_LOG_COLOR 1
#endif


/* To select the log function */
#if FML_LOG_TIMESTAMP && FML_LOG_COLOR
    #define _FML_LOG(level_str, color, fmt, ...)  \
    printf(color "[%10u][%s]" fmt FML_COLOR_RESET "\n" ,get_time(), level_str,  ##__VA_ARGS__)
#elif FML_LOG_TIMESTAMP
    #define _FML_LOG(level_str, color, fmt, ...)  \
    printf("[%10u][%s]" fmt "\n" ,get_time(), level_str,  ##__VA_ARGS__)

#elif FML_LOG_COLOR
    #define _FML_LOG(level_str, color, fmt, ...) \
    printf(color "[%s]" fmt FML_COLOR_RESET "\n" , level_str,  ##__VA_ARGS__)
#else 
    #define _FML_LOG(level_str, color, fmt, ...) \
    printf("[%s]" fmt "\n" , level_str,  ##__VA_ARGS__)
#endif  //log function


#if FML_LOG_LEVEL <= FML_LOG_LEVEL_DEBUG
    #define fml_dbg(fmt, ...) _FML_LOG("DBG", FML_COLOR_DEBUG, fmt, ##__VA_ARGS__)
#else
    #define fml_dbg(fmt, ...) ((void)0)
#endif

#if FML_LOG_LEVEL <= FML_LOG_LEVEL_INFO
    #define fml_info(fmt, ...) _FML_LOG("INF", FML_COLOR_INFO, fmt, ##__VA_ARGS__)
#else
    #define fml_info(fmt, ...) ((void)0)
#endif

#if FML_LOG_LEVEL <= FML_LOG_LEVEL_WARN
    #define fml_warn(fmt, ...) _FML_LOG("WRN", FML_COLOR_WARN, fmt, ##__VA_ARGS__)
#else
    #define fml_warn(fmt, ...) ((void)0)
#endif

/* Error log is always enabled */ 
#define fml_err(fmt, ...) _FML_LOG("ERR", FML_COLOR_ERROR, fmt, ##__VA_ARGS__)



