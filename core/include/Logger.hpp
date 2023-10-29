#pragma once
#include <iostream>
namespace freq
{
    class Logger
    {
    public:
        template <typename T>
        Logger &operator<<(const T &obj)
        {
            std::cout << obj << std::endl;
            return *this;
        }
    };

    Logger &LOG();
}