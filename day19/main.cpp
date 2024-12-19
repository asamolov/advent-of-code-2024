#define _REGEX_MAX_COMPLEXITY_COUNT 0
#define _REGEX_MAX_STACK_COUNT 0

#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"
#include <regex>
#include <map>

class task
{
private:
    std::filesystem::path file;

public:
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    template <class T>
    uint64_t match(T from, T end, std::map<int, uint64_t> &memo)
    {
        if (from == end)
        {
            return 1;
        }
        auto remaining = std::distance(from, end);
        if (!memo.contains(remaining))
        {
            for (auto &towel : towels)
            {
                if (towel.size() <= remaining && std::equal(towel.cbegin(), towel.cend(), from))
                {
                    memo[remaining] += match(std::next(from, towel.size()), end, memo);
                }
            }
        }
        return memo[remaining];
    }

    std::vector<std::string> towels;
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        std::getline(infile, s);

        // conver csv to regex
        const std::regex ws_comma(",\\s*");
        std::copy(std::sregex_token_iterator(s.begin(), s.end(), ws_comma, -1),
                  std::sregex_token_iterator(),
                  std::back_inserter(towels));

        std::sort(towels.begin(), towels.end(), [](const std::string &first, const std::string &second)
                  { return first.size() > second.size(); });
        // std::string towel_regex;
        // towel_regex.append("(");
        // auto it = towels.cbegin();
        // towel_regex.append(*it);
        // ++it;
        // for (; it != towels.cend(); ++it)
        // {
        //     towel_regex.append("|");
        //     towel_regex.append(*it);
        // }
        // towel_regex.append(")*");
        // std::regex towels_pattern(towel_regex);
        // empty line
        std::getline(infile, s);

        uint64_t variants = 0;
        while (std::getline(infile, s))
        {
            // if (std::regex_match(s, towels_pattern)) {
            std::map<int, uint64_t> memo;
            if (match(s.cbegin(), s.cend(), memo))
            {
                fmt::println("+ match   {}", s);
                n++;
                variants += memo[s.size()];
            }
            else
            {
                fmt::println("- unmatch {}", s);
            }
        }
        fmt::println("possible designs - {}, all variants - {}", n, variants);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day19" << std::endl;
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
