#pragma once

#include <platform/atomic.h>
#include <platform/mutex.hpp>
#include <platform/log.h>

#include "topic_node.hpp"

#if defined(FML_ZEPHYR)
#include <zephyr/kernel.h>

#elif defined(FML_LINUX)
#include <cstring>

#endif

#define msg(x) "x"

#define TOPIC_NUM 10

class IdcManager
{
public:
    static IdcManager &instance();

    TopicNode *advertise(const char *name, size_t size)
    {
        TopicNode *node = find_topic(name);
        if (node)
        {
            if (node->get_size() != size)
            {
                fml_error("Topic already exist but not same: %s\n", name);
                return nullptr;
            }
            return node;
        }

        f_mutex lock(&idcmanager_mutex);

        TopicNode *node_ = new TopicNode(name, size);
        if (node_ != nullptr && current_topic_index < TOPIC_NUM)
            topic_list[current_topic_index++] = node_;
        else
        {
            delete node_;
            node_ = nullptr;
        }

        return node_;
    }

    TopicNode *find_topic(const char *name)
    {
        f_mutex lock(&idcmanager_mutex);

        TopicNode *node = find_topic_in_list(name);

        return node;
    }

    void print_all_topics()
    {
        fml_info("All topics (%zu):\n", current_topic_index);

        for (size_t i = 0; i < current_topic_index; i++)
        {
            fml_info("  [%zu] %s (%zu bytes)\n", i,
                     topic_list[i]->get_name(),
                     topic_list[i]->get_size());
        }
    }

    size_t get_topic_num()
    {
        return current_topic_index;
    }

    IdcManager()
    {
        for (int i = 0; i < TOPIC_NUM; i++)
        {
            topic_list[i] = nullptr;
        }

        mutex_init(&idcmanager_mutex);
    }

    ~IdcManager()
    {
        for (int i = 0; i < current_topic_index; i++)
        {
            delete topic_list[i];
        }
    }

    IdcManager(const IdcManager &) = delete;
    IdcManager &operator=(const IdcManager &) = delete;

private:
    TopicNode *find_topic_in_list(const char *name)
    {
        for (int i = 0; i < current_topic_index; i++)
        {
            if (strcmp(topic_list[i]->get_name(), name) == 0)
            {
                return topic_list[i];
            }
        }
        return nullptr;
    }

    TopicNode *topic_list[TOPIC_NUM];
    uint8_t current_topic_index{0};
    mutex_t idcmanager_mutex;
};

// 使用 inline 确保全局唯一实例 (C++17)
inline IdcManager g_idc_manager;

inline IdcManager &IdcManager::instance()
{
    return g_idc_manager;
}