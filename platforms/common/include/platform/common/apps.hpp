
#pragma once

/*
    Header file

*/
#include <platform/atomic.hpp>

#ifdef __cplusplus

namespace fml
{
    template<class T>
    class AppBase
    {
    public:
        app() = default;
        virtual ~app() = default; //why the Destructor with virtual
        
        virtual void init() {};

        virtual void start() = 0

        virtual void stop() {};

    private:

        fml::atomic_bool is_exit{false};

    };

#endif

}