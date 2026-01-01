#pragma once

#include <type_traits>

/**
 * @file type_traits.hpp
 * @brief FML 扩展类型特征工具集
 *
 * 提供编译期类型检测和元编程工具，用于实现零开销抽象
 */

namespace fml {

// ============================================================================
// 函数存在性检测
// ============================================================================

/**
 * @brief 检测类 T 是否有静态成员函数 init()
 * @usage if constexpr (has_static_init_v<MyClass>) { MyClass::init(); }
 */
template<typename T, typename = void>
struct has_static_init : std::false_type {};

template<typename T>
struct has_static_init<T, std::void_t<decltype(T::init())>>
    : std::true_type {};

template<typename T>
inline constexpr bool has_static_init_v = has_static_init<T>::value;


/**
 * @brief 检测类 T 是否有成员函数 init()
 * @usage if constexpr (has_member_init_v<MyClass>) { obj->init(); }
 */
template<typename T, typename = void>
struct has_member_init : std::false_type {};

template<typename T>
struct has_member_init<T, std::void_t<decltype(std::declval<T>().init())>>
    : std::true_type {};

template<typename T>
inline constexpr bool has_member_init_v = has_member_init<T>::value;


/**
 * @brief 检测类 T 是否有成员函数 run()
 * @usage if constexpr (has_run_v<MyClass>) { obj->run(); }
 */
template<typename T, typename = void>
struct has_run : std::false_type {};

template<typename T>
struct has_run<T, std::void_t<decltype(std::declval<T>().run())>>
    : std::true_type {};

template<typename T>
inline constexpr bool has_run_v = has_run<T>::value;


/**
 * @brief 检测类 T 是否有成员函数 cleanup()
 * @usage if constexpr (has_cleanup_v<MyClass>) { obj->cleanup(); }
 */
template<typename T, typename = void>
struct has_cleanup : std::false_type {};

template<typename T>
struct has_cleanup<T, std::void_t<decltype(std::declval<T>().cleanup())>>
    : std::true_type {};

template<typename T>
inline constexpr bool has_cleanup_v = has_cleanup<T>::value;


// ============================================================================
// 通用函数检测宏（用于快速定义新的检测器）
// ============================================================================

/**
 * @brief 定义成员函数检测器的宏
 * @example FML_DEFINE_HAS_MEMBER_FUNCTION(on_event)
 *          然后可以使用 has_on_event_v<T>
 */
#define FML_DEFINE_HAS_MEMBER_FUNCTION(func_name)                           \
    template<typename T, typename = void>                                   \
    struct has_##func_name : std::false_type {};                           \
                                                                            \
    template<typename T>                                                    \
    struct has_##func_name<T, std::void_t<decltype(std::declval<T>().func_name())>> \
        : std::true_type {};                                                \
                                                                            \
    template<typename T>                                                    \
    inline constexpr bool has_##func_name##_v = has_##func_name<T>::value;


/**
 * @brief 定义静态函数检测器的宏
 */
#define FML_DEFINE_HAS_STATIC_FUNCTION(func_name)                           \
    template<typename T, typename = void>                                   \
    struct has_static_##func_name : std::false_type {};                    \
                                                                            \
    template<typename T>                                                    \
    struct has_static_##func_name<T, std::void_t<decltype(T::func_name())>> \
        : std::true_type {};                                                \
                                                                            \
    template<typename T>                                                    \
    inline constexpr bool has_static_##func_name##_v = has_static_##func_name<T>::value;


// ============================================================================
// 成员变量检测
// ============================================================================

/**
 * @brief 检测类 T 是否有成员变量 cycle
 */
template<typename T, typename = void>
struct has_member_cycle : std::false_type {};

template<typename T>
struct has_member_cycle<T, std::void_t<decltype(std::declval<T>().cycle)>>
    : std::true_type {};

template<typename T>
inline constexpr bool has_member_cycle_v = has_member_cycle<T>::value;


// ============================================================================
// 类型关系检测
// ============================================================================

/**
 * @brief 检测 T 是否是 Base 的派生类（比 std::is_base_of 更易读）
 */
template<typename Base, typename T>
inline constexpr bool is_derived_from_v = std::is_base_of<Base, T>::value;

/**
 * @brief 检测 T 是否继承自 Base（别名，语义更清晰）
 * @usage if constexpr (is_base_of_v<WorkItem, T>) { ... }
 */
template<typename Base, typename T>
inline constexpr bool is_base_of_v = std::is_base_of<Base, T>::value;


/**
 * @brief 检测 T 是否可以被调用（函数、lambda、函数对象）
 */
template<typename T, typename = void>
struct is_callable : std::false_type {};

template<typename T>
struct is_callable<T, std::void_t<decltype(&T::operator())>>
    : std::true_type {};

// 特化：函数指针
template<typename Ret, typename... Args>
struct is_callable<Ret(*)(Args...)> : std::true_type {};

template<typename T>
inline constexpr bool is_callable_v = is_callable<T>::value;


} // namespace fml
