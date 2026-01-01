
#pragma once

#include "idcmanager.hpp"

template <typename T>
class Subscriber
{
public:
    Subscriber(const char *name) : topic_name(name), topic(nullptr), generation(0)
    {
        try_init();
    }

    bool try_init()
    {
        if (topic != nullptr)
            return true; // 已经初始化

        TopicNode *topic_ = IdcManager::instance().find_topic(topic_name);
        if (topic_ != nullptr)
        {
            if (topic_->get_size() == sizeof(T))
            {
                topic = topic_;
                return true;
            }
            else
            {
                fml_error("Topic '%s' size mismatch: expected %zu, got %zu\n",
                          topic_name, sizeof(T), topic_->get_size());
            }
        }
        return false;
    }

    bool copy(T *data)
    {
        if (!topic)
            return false;
        return topic->subscribe(data, generation);
    }

    bool update()
    {
        if (!topic)
            return false;
        return topic->update(generation);
    }

    bool is_valid()
    {
        return topic != nullptr;
    }

    ~Subscriber() = default;

private:
    const char *topic_name;
    TopicNode *topic;
    unsigned generation{0};
};
