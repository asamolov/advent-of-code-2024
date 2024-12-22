#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include <map>
#include <array>

using seq = std::array<int, 4>;

class task
{
private:
    std::filesystem::path file;
    std::vector<std::vector<int>> prices;
    std::vector<std::map<seq, int>> caches;

public:
    uint32_t pseudo_random(uint32_t input, uint8_t iteration)
    {
        auto expanded_input = static_cast<uint64_t>(input);
        expanded_input = ((expanded_input << 6) ^ expanded_input) & ((1 << 24) - 1);
        expanded_input = ((expanded_input >> 5) ^ expanded_input) & ((1 << 24) - 1);
        expanded_input = ((expanded_input << 11) ^ expanded_input) & ((1 << 24) - 1);
        return static_cast<uint32_t>(expanded_input);
    }
    seq shift(const seq &old, int el)
    {
        seq result;
        for (size_t i = 1; i < old.size(); i++)
        {
            result[i - 1] = old[i];
        }
        result.back() = el;
        return result;
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
            std::vector<int> price_line;
            std::map<seq, int> cache;
            price_line.push_back(t % 10);
            seq key = {-100, -100, -100, -100};
            int prev_price = t % 10;
            for (int i = 0; i < 2000; i++)
            {
                rnd = pseudo_random(rnd, i);
                int price = rnd % 10;
                price_line.push_back(price);
                int delta = price - prev_price;
                prev_price = price;
                key = shift(key, delta);
                if (!cache.contains(key))
                { // first occurence of the key
                    cache[key] = price;
                }
            }
            caches.push_back(std::move(cache));
            prices.push_back(std::move(price_line));
            fmt::println("{}: {}", t, rnd);
            n += rnd;
        }

        seq debug = {-2,1,-1,3};
        for (auto &&cache : caches)
        {
            auto it = cache.find(debug);
            if (it != cache.end())
            {
                fmt::println("Found {}", it->second);
            } else {
                fmt::println("not found");
            }
        }

        // enumerating
        int max_sum = 0;
        int iter = 0;
        for (int i = -9; i < 10; i++)
        {
            for (int j = -9; j < 10; j++)
            {
                for (int k = -9; k < 10; k++)
                {
                    for (int l = -9; l < 10; l++)
                    {
                        if (iter++ % 1000 == 0)
                        {
                            fmt::println("Iteration {}...", iter);
                        }
                        seq key = {i, j, k, l};
                        int sum = 0;
                        for (auto &&cache : caches)
                        {
                            auto it = cache.find(key);
                            if (it != cache.end())
                            {
                                sum += it->second;
                            }
                        }
                        if (sum > max_sum)
                        {
                            max_sum = sum;
                            fmt::println("Better sum {} with seq {}", max_sum, key);
                        }
                    }
                }
            }
        }

        fmt::println("sum - {}, max bananas - {}", n, max_sum);
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
