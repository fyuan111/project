#pragma once

#include <stdint.h>
#include <utility>  

namespace fml {

template<typename T>
class MinHeap
{
public:
    /**
     * @brief 默认比较器（最小堆）
     */
    template<typename U>
    struct DefaultCompare {
        bool operator()(const U& a, const U& b) const {
            return a < b;
        }
    };

    /**
     * @brief 上浮操作
     * @param heap 堆数组
     * @param idx 起始索引
     * @param compare 比较器
     */
    template<typename Compare = DefaultCompare<T>>
    static void sift_up(T* heap, int idx, Compare compare = Compare())
    {
        T item = heap[idx];

        while (idx > 0)
        {
            int parent = (idx - 1) / 2;

            if (!compare(item, heap[parent]))
            {
                break;
            }

            heap[idx] = heap[parent];
            idx = parent;
        }

        heap[idx] = item;
    }

    /**
     * @brief 下沉操作
     * @param heap 堆数组
     * @param idx 起始索引
     * @param size 堆大小
     * @param compare 比较器
     */
    template<typename Compare = DefaultCompare<T>>
    static void sift_down(T* heap, int idx, int size, Compare compare = Compare())
    {
        T item = heap[idx];
        int half = size / 2;

        while (idx < half)
        {
            int child = 2 * idx + 1;
            int right = child + 1;

            if (right < size && compare(heap[right], heap[child]))
            {
                child = right;
            }

            if (!compare(heap[child], item))
            {
                break;
            }

            heap[idx] = heap[child];
            idx = child;
        }

        heap[idx] = item;
    }

    /**
     * @brief 插入元素到堆
     * @param heap 堆数组
     * @param size 堆大小（引用，会修改）
     * @param value 要插入的元素
     * @param max_size 最大容量
     * @param compare 比较器
     * @return 成功返回 true，堆满返回 false
     */
    template<typename Compare = DefaultCompare<T>>
    static bool push(T* heap, int& size, T value, int max_size, Compare compare = Compare())
    {
        if (size >= max_size)
            return false;

        heap[size] = value;
        sift_up(heap, size, compare);
        size++;
        return true;
    }

    /**
     * @brief 弹出堆顶（最小元素）
     * @param heap 堆数组
     * @param size 堆大小（引用，会修改）
     * @param compare 比较器
     * @return 堆顶元素
     * @warning 调用前需确保 size > 0
     */
    template<typename Compare = DefaultCompare<T>>
    static T pop(T* heap, int& size, Compare compare = Compare())
    {
        T result = heap[0];

        size--;
        if (size > 0)
        {
            heap[0] = heap[size];
            sift_down(heap, 0, size, compare);
        }

        return result;
    }

    /**
     * @brief 查看堆顶元素（不删除）
     * @param heap 堆数组
     * @param size 堆大小
     * @return 堆顶元素，空堆返回默认值
     */
    static T peek(T* heap, int size)
    {
        return (size > 0) ? heap[0] : T();
    }

    /**
     * @brief 建堆（Floyd 算法，O(n) 复杂度）
     * @param heap 堆数组
     * @param size 堆大小
     * @param compare 比较器
     */
    template<typename Compare = DefaultCompare<T>>
    static void heapify(T* heap, int size, Compare compare = Compare())
    {
        for (int i = (size / 2) - 1; i >= 0; i--)
        {
            sift_down(heap, i, size, compare);
        }
    }

    /**
     * @brief 检查是否为有效的堆
     * @param heap 堆数组
     * @param size 堆大小
     * @param compare 比较器
     * @return true 如果满足堆性质
     */
    template<typename Compare = DefaultCompare<T>>
    static bool is_heap(T* heap, int size, Compare compare = Compare())
    {
        for (int i = 0; i < size; i++)
        {
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < size && compare(heap[left], heap[i]))
                return false;

            if (right < size && compare(heap[right], heap[i]))
                return false;
        }
        return true;
    }
};

} // namespace fml
