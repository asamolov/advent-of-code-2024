#pragma once
#include <iostream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <source_location>

void usage(char *path)
{
    std::cout << "Usage: " << path << " <input.txt>" << std::endl;
    std::cout << "where <input.txt> - file with the command input" << std::endl;
}

bool ensure_input(int argc, char *argv[], std::filesystem::path &input)
{
    if (argc == 2)
    {
        input.assign(argv[1]);
        if (std::filesystem::exists(input))
        {
            return true;
        }
    }
    return false;
}

template <class Resolution = std::chrono::milliseconds>
class execution_timer
{
public:
    using clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                                     std::chrono::high_resolution_clock,
                                     std::chrono::steady_clock>;

private:
    const clock::time_point start = clock::now();
    const std::string fn;
public:
    execution_timer(const std::source_location& location = std::source_location::current()) : fn(location.function_name()) {
    }
    ~execution_timer()
    {
        const auto end = clock::now();
        fmt::println("total {} elapsed: {}", fn, std::chrono::duration_cast<Resolution>(end - start));
    }

    inline void stop(const std::string &tag = "!")
    {
        const auto end = clock::now();
        fmt::println("{} elapsed: {}", tag, std::chrono::duration_cast<Resolution>(end - start));
    }

}; // ExecutionTimer