#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

class task
{
private:
    std::filesystem::path file;

public:
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    std::string run()
    {
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        while (std::getline(infile, s))
        {
            std::istringstream is(s);
            int prev_level = 0;
            if (!(is >> prev_level))
            {
                continue;
            }
            int level = 0;
            if (!(is >> level))
            {
                continue;
            }
            int delta = prev_level - level;
            int dir = sgn(delta);
            bool safe = true;
            do
            {
                delta = prev_level - level;
                if (dir != sgn(delta) 
                    || std::abs(delta) < 1 
                    || std::abs(delta) > 3)
                {
                    safe = false;
                    break;
                }
                prev_level = level;
            } while (is >> level);
            n += safe;
        }
        return fmt::format("safe reports - {}", n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day02" << std::endl;
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
        auto result = task.run();
        std::cout << "Result: " << result << std::endl;
    }
    else
    {
        usage(argv[0]);
    }
    return 0;
}
