#include "Logger.hpp"

namespace freq
{
    Logger &LOG()
    {
        static Logger logger;
        return logger;
    }
}