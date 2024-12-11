#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include "../BigInt.hpp"
#include <list>
#include <map>

const BigInt zero("0");
const BigInt one("1");

class task
{
private:
    std::filesystem::path file;
    std::map<BigInt, std::vector<unsigned long long>> cache;

public:
    unsigned long long get_power(const BigInt &x, int n)
    {
        auto &power = cache[x];
        if (power.size() == 0)
        {
            power.push_back(1);
        }
        if (n >= power.size())
        {
            // do split
            if (x == zero)
            {
                auto next = one;
                get_power(next, n - 1);
                // copy power from child
                copy_power(next, x);
            }
            else
            {
                auto chars = n_chars(x);
                if (chars % 2 == 0)
                {
                    auto p = big_pow10(chars / 2);
                    auto left = x / p;
                    auto right = x % p;
                    
                    get_power(left, n - 1);
                    get_power(right, n - 1);
                    // copy power from children
                    auto &left_child = cache[left];
                    auto &right_child = cache[right];
                    auto &power = cache[x];
                    auto missing_offset = power.size() - 1;
                    for (size_t i = missing_offset; i < std::min(left_child.size(), right_child.size()); i++)
                    {
                        power.push_back(left_child[i] + right_child[i]);
                    }
                }
                else
                {
                    auto next = x * 2024;
                    get_power(next, n - 1);
                    // copy power from child
                    copy_power(next, x);
                }
            }
        }

        return cache[x][n];
    }
    void copy_power(BigInt &from, const BigInt &to)
    {
        auto &child = cache[from];
        auto &power = cache[to];
        auto missing_offset = power.size() - 1;
        for (size_t i = missing_offset; i < child.size(); i++)
        {
            power.push_back(child[i]);
        }
    }
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    size_t n_chars(const BigInt &n)
    {
        return n.to_string().size();
    }
    BigInt pow(int n)
    {
        BigInt result = 1;
        while (n > 0)
        {
            result *= 10;
            n--;
        }
        return result;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        unsigned long long deep_25 = 0;
        unsigned long long deep_75 = 0;
        BigInt n = 0;
        while (infile >> n)
        {
            deep_75 += get_power(n, 75);
            deep_25 += get_power(n, 25);
        }
        fmt::println("result: 25 steps - {}, 75 steps - {}", deep_25, deep_75);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day11" << std::endl;
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
