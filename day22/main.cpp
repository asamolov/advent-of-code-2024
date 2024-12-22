#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"

class task
{
private:
    std::filesystem::path file;

public:
    uint32_t pseudo_random(uint32_t input, uint8_t iteration)
    {
        auto expanded_input = static_cast<uint64_t>(input);
        expanded_input = ((expanded_input << 6) ^ expanded_input) & ((1 << 24) - 1);
        expanded_input = ((expanded_input >> 5) ^ expanded_input) & ((1 << 24) - 1);
        expanded_input = ((expanded_input << 11) ^ expanded_input) & ((1 << 24) - 1);
        return static_cast<uint32_t>(expanded_input);
    }
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        uint64_t n = 0;
        while (std::getline(infile, s))
        {
            uint32_t t = std::stoul(s);
            uint32_t rnd = t;
            for (int i = 0; i < 2000; i++)
            {
                rnd = pseudo_random(rnd, i);
            }
            fmt::println("{}: {}", t, rnd);
            n += rnd;
        }
        fmt::println("sum - {}", n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day22" << std::endl;
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
    std::cout << "args:" << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << '\t' << argv[i] << std::endl;
    }
    std::filesystem::path input;
    if (ensure_input(argc, argv, input))
    {
        task task{input};
        task.run();
    }
    else
    {
        usage(argv[0]);
    }
    return 0;
}
