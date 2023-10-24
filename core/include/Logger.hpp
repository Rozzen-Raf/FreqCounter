#pragma once
#include <iostream>
namespace freq
{
    class Logger
    {
    public:
        template <typename T>
        void operator<<(const T &obj)
        {
            std::cout << obj << std::endl;
        }
    };

    Logger &LOG();
}