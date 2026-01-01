
#pragma once

#include <platform/atomic.h>
#include <platform/mutex.hpp>
#include "idcmanager.hpp"

template <typename T>
class Publisher
{
public:
    explicit Publisher(const char *name) : _topic(nullptr)
    {
        if (!name)
        {
            fml_error("Publisher: name is null\n");
            return;
        }
        _topic = IdcManager::instance().advertise(name, sizeof(T));
        if (!_topic)
        {
            fml_error("Publisher: failed to advertise topic: %s\n", name);
        }
    }

    bool publish(const T &data)
    {
        if (!_topic)
            return false;
        return _topic->publish(&data);
    }

    bool valid() const
    {
        return _topic != nullptr;
    }

    ~Publisher() = default;

private:
    TopicNode *_topic;
};
