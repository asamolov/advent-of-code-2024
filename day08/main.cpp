#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include <map>
#include <set>

using point = std::pair<int, int>;

class task
{
private:
    std::filesystem::path file;

public:
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
        std::vector<std::string> field;
        std::vector<std::string> antinodes;
        std::set<char> keys;
        std::multimap<char, point> nodes;
        int n = 0;
        while (std::getline(infile, s))
        {
            for (size_t i = 0; i < s.size(); i++)
            {
                char ch = s.at(i);
                if (ch != '.')
                {
                    nodes.emplace(ch, point{field.size(), i});
                    keys.insert(ch);
                }
            }
            field.push_back(s);
            antinodes.push_back(std::string(s.size(), '.'));
        }
        int width = field[0].size();
        int height = field.size();
        int anti_n = 0;
        fmt::println("field:\n{}", fmt::join(field, "\n"));
        for (auto &e : nodes)
        {
            fmt::println("{} => {}", e.first, e.second);
        }
        // handling nodes
        for (auto k : keys)
        {
            fmt::print("{} => ", k);
            auto range = nodes.equal_range(k);
            for (auto it = range.first; it != range.second; ++it)
            {
                fmt::print("{},", *it);
                auto node = it->second;
                for (auto ot = range.first; ot != range.second; ++ot)
                {
                    auto other = ot->second;
                    if (*it == *ot)
                    {
                        continue;
                    }
                    // find antinode symmetrical it -> ot -> antinode
                    auto anti_row = 2 * other.first - node.first;
                    auto anti_col = 2 * other.second - node.second;

                    if (anti_row >= 0 && anti_row < height && anti_col >= 0 && anti_col < width)
                    {
                        antinodes[anti_row][anti_col] = '#';
                    }
                }
            }
            fmt::println("");
        }
        fmt::println("antinodes:\n{}", fmt::join(antinodes, "\n"));
        
        for (auto &row : antinodes)
        {
            for(auto &col : row) {
                anti_n += col == '#';
            }
        }
        fmt::println("antinodes - {}", anti_n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day08" << std::endl;
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
