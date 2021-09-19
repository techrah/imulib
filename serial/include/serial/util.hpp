#ifndef __C7CFE287_2FC4_4FE9_B986_7D410961CA23__
#define __C7CFE287_2FC4_4FE9_B986_7D410961CA23__

#include <time.h>
#include <chrono>
#include <memory>
#include <stdint.h>
#include <stdexcept>
#include <string>

namespace serial
{
    inline void delay(unsigned long ms)
    {
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1e6;
        nanosleep(&ts, NULL);
    }

    inline uint64_t getEpochTimeMs()
    {
        const auto tm = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   tm.time_since_epoch())
            .count();
    }

    template <typename... Args>
    std::string string_format(const std::string &format, Args... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
        if (size_s <= 0)
        {
            throw std::runtime_error("Error during formatting.");
        }
        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
};

#endif
