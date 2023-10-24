#pragma once

#include <mutex>

namespace freq
{
    // Стратегия синхронизации при однопоточном выполнении
    class SingleThreadPolicy
    {
    public:
        void lock() {}
        void unlock() {}
    };

    // Стратегия синхронизации при многопоточном выполнении
    template <typename mutex_t = std::recursive_mutex>
    class MultiThreadPolicy
    {
    public:
        void lock()
        {
            mutex.lock();
        }
        void unlock()
        {
            mutex.unlock();
        }

    private:
        mutex_t mutex;
    };
}