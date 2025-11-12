#include <common/apps.hpp>
#include <platform/log.h>


class hello :public AppBase<hello>
{
public:
    hello()
    {
        fml_info("hello world");
    }

    ~hello()
    {
        fml_info("thanks");
    }
};

void hello_main(int argc, char *argv[])
{
    hello a;
    a.printf_usage();
}
