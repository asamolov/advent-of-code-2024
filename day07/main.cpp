#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include <regex>

class task
{
private:
    std::filesystem::path file;
    bool use_concat;
public:
    task(const std::filesystem::path &input, bool use_concat = false) : file(input), use_concat(use_concat)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    template <class T>
    bool check_exp(long long sum, T from, T till)
    {
        // shortcut
        if (sum < 0)
        {
            return false;
        }
        if (from == till)
        {
            return sum == 0;
        }
        bool result = false;
        long long a = *from;
        ++from;
        if (sum % a == 0)
        {
            if (check_exp(sum / a, from, till)) {
                return true;
            }
        }
        // check if it ends by current element
        if (use_concat) {
            auto str_sum = std::to_string(sum);
            auto str_curr = std::to_string(a);
            if (str_sum.ends_with(str_curr)) {
                str_sum.resize(str_sum.size() - str_curr.size());
                if (!str_sum.empty()) {
                    if (check_exp(std::stoll(str_sum), from, till)) {
                        return true;
                    }
                }
            }
        }
        return check_exp(sum - a, from, till);
    }
    auto check_line(const std::string &line)
    {
        auto idx = line.find(':');
        auto result = std::stoll(line);
        std::istringstream is(line.substr(idx + 1));
        std::vector<long long> expr;
        long long i = 0;
        while (is >> i)
        {
            expr.push_back(i);
        }
        //fmt::print("Checking {} against {}", result, expr);
        // checking from the back
        if (check_exp(result, expr.crbegin(), expr.crend()))
        {
            //fmt::println(" <- correct!");
            return result;
        }
        //fmt::println("");
        return 0LL;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        long long n = 0;
        while (std::getline(infile, s))
        {
            n += check_line(s);
        }
        fmt::println("sum - {}", n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day07" << std::endl;
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
    std::cout << "args:" << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << '\t' << argv[i] << std::endl;
    }
    std::filesystem::path input;
    if (ensure_input(argc, argv, input))
    {
        task part1{input};
        part1.run();
        task part2{input, true};
        part2.run();
    }
    else
    {
        usage(argv[0]);
    }
    return 0;
}
