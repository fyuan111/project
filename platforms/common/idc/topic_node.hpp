#pragma once

#include <stdint.h>
#include <platform/mutex.hpp>
#include <platform/atomic.h>

class TopicNode
{
public:
    TopicNode(const char *name, size_t size) : name(name), size(size), version(0)
    {
        if (mutex_init(&idc_mutex))
        {
            fml_info("mutex_init fail"); // todo
        }

        _data = new uint8_t[size];

        version.store(0);
    }

    ~TopicNode()
    {
        delete[] _data;
        mutex_destroy(&idc_mutex);
    }

    bool publish(const void *data)
    {
        f_mutex lock(&idc_mutex);

        memcpy(_data, data, size);

        version.fetch_add(1);

        return true;
    }

    bool subscribe(void *data, unsigned &generation)
    {
        f_mutex lock(&idc_mutex);

        memcpy(data, _data, size);

        generation = version.load();

        return true;
    }

    bool update(unsigned generation)
    {
        return static_cast<unsigned>(version.load()) != generation;
    }

    size_t get_size()
    {
        return size;
    }

    const char *get_name()
    {
        return name;
    }

private:
    const char *name;
    uint8_t *_data;
    size_t size;
    fml::atomic_int32_t version;
    mutex_t idc_mutex;
};