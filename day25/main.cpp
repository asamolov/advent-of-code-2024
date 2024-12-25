#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include <array>

class task
{
private:
    std::filesystem::path file;

public:
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    using pins = std::array<uint8_t, 5>;
    bool match(const pins &lock, const pins &key)
    {
        for (size_t i = 0; i < lock.size(); i++)
        {
            if (lock[i] + key[i] > 6) // 6 due to extra pin in keys
                return false;
        }
        return true;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        std::vector<pins> locks;
        std::vector<pins> keys;
        int n = 0;
        bool is_lock = false;
        while (std::getline(infile, s))
        {
            is_lock = s[0] == '#';
            pins current;
            current.fill(0);
            while (std::getline(infile, s) && !s.empty()) // keys will have + 1 to them, it's fine.
            {
                for (size_t i = 0; i < current.size(); i++)
                {
                    current[i] += s[i] == '#';
                }
            }
            if (is_lock)
            {
                locks.push_back(current);
            }
            else
            {
                keys.push_back(current);
            }
        }
        for (auto &lock : locks)
        {
            for (auto &key : keys) {
                n += match(lock, key);
            }
        }

        fmt::println("n pairs - {}", n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day25" << std::endl;
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
