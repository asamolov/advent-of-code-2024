#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include "BigInt.hpp"

class task
{
private:
    std::filesystem::path file;

public:
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    size_t n_chars(BigInt &n)
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
        std::vector<BigInt> v;
        BigInt n = 0;
        while (infile >> n)
        {
            v.push_back(n);
        }
        decltype(v) next;
        BigInt zero("0");
        BigInt one("1");
        BigInt two("2");
        BigInt ten("10");
        BigInt year("2024");
        for (size_t i = 0; i < 75; i++)
        {
            fmt::print("it {}, {} items:", i, v.size());
            
            next.clear();
            for (auto &x : v)
            {
                //fmt::print(" {}", x.to_string());
                if (x == zero)
                {
                    next.push_back(one);
                }
                else
                {
                    auto chars = n_chars(x);
                    if (chars % 2 == 0)
                    {
                        BigInt p = big_pow10(chars / 2);
                        next.push_back(x / p);
                        next.push_back(x % p);
                    }
                    else
                    {
                        auto mult = x * 2024;
                        next.push_back(mult);
                    }
                }
            }
            fmt::println("");
            v.swap(next);
        }

        fmt::println("stones - {}", v.size());
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
