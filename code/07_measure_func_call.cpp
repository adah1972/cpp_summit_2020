// g++ -O2 -std=c++17 -pthread 07_measure_func_call.cpp profiler.cpp
//

#include <cctype>      // std::isspace
#include <cstddef>     // std::size_t
#include <memory>      // std::make_unique
#include <mutex>       // std::mutex
#include "profiler.h"  // PROFILE_CHECK

enum profiled_functions {
    PF_COUNT_SPACE,
    PF_COUNT_SPACE_NOINLINE,
    PF_COUNT_SPACE_VIRTUAL,
};

name_mapper name_map[] = {
    {PF_COUNT_SPACE, "count_space"},
    {PF_COUNT_SPACE_NOINLINE, "count_space_noinline"},
    {PF_COUNT_SPACE_VIRTUAL, "count_space_virtual"},
    {-1, NULL}};

class space_checker {
public:
    bool isspace(char ch)
    {
        return std::isspace(ch);
    }
};

class space_checker_noinline {
public:
    bool isspace(char ch);
};

__attribute__((noinline)) bool space_checker_noinline::isspace(char ch)
{
    return std::isspace(ch);
}

class space_checker_intf {
public:
    virtual ~space_checker_intf() = default;
    virtual bool isspace(char ch) = 0;
};

// The purpose of this class is prevent GCC devirtualization optimization
class dummy : public space_checker_intf {
public:
    bool isspace(char ch) override;
};

bool dummy::isspace(char /*ch*/)
{
    return false;
}

class space_checker_virt : public space_checker_intf {
public:
    bool isspace(char ch) override;
};

bool space_checker_virt::isspace(char ch)
{
    return std::isspace(ch);
}

std::size_t count_space(const char* str, space_checker& checker)
{
    PROFILE_CHECK(PF_COUNT_SPACE);
    std::size_t count = 0;
    while (*str != '\0') {
        if (checker.isspace(*str)) {
            ++count;
        }
        ++str;
    }
    return count;
}

std::size_t count_space_noinline(const char* str,
                                 space_checker_noinline& checker)
{
    PROFILE_CHECK(PF_COUNT_SPACE_NOINLINE);
    std::size_t count = 0;
    while (*str != '\0') {
        if (checker.isspace(*str)) {
            ++count;
        }
        ++str;
    }
    return count;
}

std::size_t count_space_virtual(const char* str,
                                space_checker_intf& checker)
{
    PROFILE_CHECK(PF_COUNT_SPACE_VIRTUAL);
    std::size_t count = 0;
    while (*str != '\0') {
        if (checker.isspace(*str)) {
            ++count;
        }
        ++str;
    }
    return count;
}

std::size_t result;
std::mutex mutex;
std::unique_ptr<space_checker_intf> virt_checker;

#define LOOPS 10000

int main()
{
    const char input[] = "Hello world Hello world Hello world Hello world";

    {
        space_checker checker;
        for (int i = 0; i < LOOPS; ++i) {
            std::lock_guard guard(mutex);
            result = count_space(input, checker);
        }
    }

    {
        space_checker_noinline checker;
        for (int i = 0; i < LOOPS; ++i) {
            std::lock_guard guard(mutex);
            result = count_space_noinline(input, checker);
        }
    }

    {
        virt_checker = std::make_unique<space_checker_virt>();
        for (int i = 0; i < LOOPS; ++i) {
            std::lock_guard guard(mutex);
            result = count_space_virtual(input, *virt_checker);
        }
    }
}
