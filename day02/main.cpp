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
    bool is_safe(const std::vector<int> &input, int exclude = -1)
    {
        if (input.size() < 2) {
            return false;
        }
        int prev_level = -1;
        int dir = 111;
        for (auto it = input.cbegin(); it != input.cend(); ++it) {
            // skip the exclude element
            if (exclude == std::distance(input.cbegin(), it)) {
                continue;
            }
            // first element
            if (prev_level < 0) {
                prev_level = *it;
                continue;
            }
            int delta = prev_level - *it;
            // second element
            if (dir > 1) {
                dir = sgn(delta); // initial direction
            }
            // rest
            if (dir != sgn(delta) || std::abs(delta) < 1 || std::abs(delta) > 3)
            {
                return false;
            }
            prev_level = *it;
        }
        return true;
    }

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
            int level = 0;
            std::vector<int> levels;
            while (is >> level) {
                levels.push_back(level);
            }
            n += is_safe(levels);
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
