#include "../heap.hpp"
#include "test_utils.hpp"

using namespace fml;

// ============ 测试数据结构 ============

struct TestNode {
    int value;
    TestNode(int v) : value(v) {}
};

// 比较器：按 value 升序
struct NodeCompare {
    bool operator()(TestNode* a, TestNode* b) const {
        return a->value < b->value;
    }
};

// ============ 测试用例 ============

/**
 * @brief 测试基本 push/pop
 */
bool test_basic_push_pop()
{
    int* heap[10];
    int size = 0;

    // 比较器：比较指针指向的值
    auto cmp = [](int* a, int* b) { return *a < *b; };

    // Push 3 个元素
    MinHeap<int*>::push(heap, size, new int(5), 10, cmp);
    MinHeap<int*>::push(heap, size, new int(3), 10, cmp);
    MinHeap<int*>::push(heap, size, new int(7), 10, cmp);

    TEST_ASSERT_EQ(size, 3, "Size should be 3");

    // Pop 应该按升序返回
    int* top1 = MinHeap<int*>::pop(heap, size, cmp);
    TEST_ASSERT_EQ(*top1, 3, "First pop should be 3");

    int* top2 = MinHeap<int*>::pop(heap, size, cmp);
    TEST_ASSERT_EQ(*top2, 5, "Second pop should be 5");

    int* top3 = MinHeap<int*>::pop(heap, size, cmp);
    TEST_ASSERT_EQ(*top3, 7, "Third pop should be 7");

    TEST_ASSERT_EQ(size, 0, "Size should be 0 after all pops");

    // 清理
    delete top1;
    delete top2;
    delete top3;

    return true;
}

/**
 * @brief 测试 peek（不删除）
 */
bool test_peek()
{
    int* heap[10];
    int size = 0;

    auto cmp = [](int* a, int* b) { return *a < *b; };

    MinHeap<int*>::push(heap, size, new int(10), 10, cmp);
    MinHeap<int*>::push(heap, size, new int(5), 10, cmp);

    int* top = MinHeap<int*>::peek(heap, size);
    TEST_ASSERT_EQ(*top, 5, "Peek should return 5");
    TEST_ASSERT_EQ(size, 2, "Size should still be 2 after peek");

    // 清理
    while (size > 0) {
        delete MinHeap<int*>::pop(heap, size, cmp);
    }

    return true;
}

/**
 * @brief 测试 heapify（建堆）
 */
bool test_heapify()
{
    int* heap[10];
    heap[0] = new int(9);
    heap[1] = new int(5);
    heap[2] = new int(7);
    heap[3] = new int(1);
    heap[4] = new int(3);
    int size = 5;

    // 建堆
    auto compare = [](int* a, int* b) { return *a < *b; };
    MinHeap<int*>::heapify(heap, size, compare);

    // 验证堆性质
    TEST_ASSERT(MinHeap<int*>::is_heap(heap, size, compare), "Should be a valid heap");

    // Pop 应该按升序
    int prev = -1;
    while (size > 0) {
        int* current = MinHeap<int*>::pop(heap, size, compare);
        TEST_ASSERT(*current >= prev, "Pop order should be ascending");
        prev = *current;
        delete current;
    }

    return true;
}

/**
 * @brief 测试自定义比较器
 */
bool test_custom_comparator()
{
    TestNode* heap[10];
    int size = 0;

    NodeCompare cmp;

    MinHeap<TestNode*>::push(heap, size, new TestNode(50), 10, cmp);
    MinHeap<TestNode*>::push(heap, size, new TestNode(20), 10, cmp);
    MinHeap<TestNode*>::push(heap, size, new TestNode(80), 10, cmp);
    MinHeap<TestNode*>::push(heap, size, new TestNode(10), 10, cmp);

    // Pop 应该按 value 升序
    TestNode* n1 = MinHeap<TestNode*>::pop(heap, size, cmp);
    TEST_ASSERT_EQ(n1->value, 10, "First should be 10");

    TestNode* n2 = MinHeap<TestNode*>::pop(heap, size, cmp);
    TEST_ASSERT_EQ(n2->value, 20, "Second should be 20");

    // 清理
    delete n1;
    delete n2;
    while (size > 0) {
        delete MinHeap<TestNode*>::pop(heap, size, cmp);
    }

    return true;
}

/**
 * @brief 测试堆满情况
 */
bool test_heap_full()
{
    int* heap[3];
    int size = 0;

    auto cmp = [](int* a, int* b) { return *a < *b; };

    // 填满
    TEST_ASSERT(MinHeap<int*>::push(heap, size, new int(1), 3, cmp), "Push 1 should succeed");
    TEST_ASSERT(MinHeap<int*>::push(heap, size, new int(2), 3, cmp), "Push 2 should succeed");
    TEST_ASSERT(MinHeap<int*>::push(heap, size, new int(3), 3, cmp), "Push 3 should succeed");

    // 再 push 应该失败
    TEST_ASSERT(!MinHeap<int*>::push(heap, size, new int(4), 3, cmp), "Push 4 should fail (full)");

    TEST_ASSERT_EQ(size, 3, "Size should still be 3");

    // 清理
    while (size > 0) {
        delete MinHeap<int*>::pop(heap, size, cmp);
    }

    return true;
}

/**
 * @brief 测试大量元素
 */
bool test_large_heap()
{
    constexpr int N = 100;
    int* heap[N];
    int size = 0;

    auto cmp = [](int* a, int* b) { return *a < *b; };

    // 逆序插入
    for (int i = N - 1; i >= 0; i--) {
        MinHeap<int*>::push(heap, size, new int(i), N, cmp);
    }

    TEST_ASSERT_EQ(size, N, "Should have 100 elements");

    // Pop 应该是升序 0, 1, 2, ..., 99
    for (int i = 0; i < N; i++) {
        int* val = MinHeap<int*>::pop(heap, size, cmp);
        TEST_ASSERT_EQ(*val, i, "Should pop in ascending order");
        delete val;
    }

    TEST_ASSERT_EQ(size, 0, "Should be empty");

    return true;
}

/**
 * @brief 测试 is_heap 验证函数
 */
bool test_is_heap_validation()
{
    int* heap[5];
    heap[0] = new int(1);
    heap[1] = new int(3);
    heap[2] = new int(2);
    heap[3] = new int(5);
    heap[4] = new int(4);
    int size = 5;

    auto compare = [](int* a, int* b) { return *a < *b; };

    // 这是一个有效的最小堆
    TEST_ASSERT(MinHeap<int*>::is_heap(heap, size, compare), "Should be a valid heap");

    // 破坏堆性质
    int* temp = heap[0];
    heap[0] = heap[4];
    heap[4] = temp;

    TEST_ASSERT(!MinHeap<int*>::is_heap(heap, size, compare), "Should not be a valid heap");

    // 清理
    for (int i = 0; i < size; i++) {
        delete heap[i];
    }

    return true;
}

// ============ 主函数 ============

int main()
{
    int total = 0, passed = 0, failed = 0;

    printf("\n========== MinHeap Unit Tests ==========\n\n");

    RUN_TEST(test_basic_push_pop);
    RUN_TEST(test_peek);
    RUN_TEST(test_heapify);
    RUN_TEST(test_custom_comparator);
    RUN_TEST(test_heap_full);
    RUN_TEST(test_large_heap);
    RUN_TEST(test_is_heap_validation);

    TEST_SUMMARY();

    return (failed == 0) ? 0 : 1;
}
