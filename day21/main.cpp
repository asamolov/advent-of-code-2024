#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"
#include <array>
#include <map>

using dir = aoc::dir;
using point = aoc::point<size_t>;

const std::array num_keypad = {'7', '8', '9',
                               '4', '5', '6',
                               '1', '2', '3',
                               'X', '0', 'A'};

const std::array dir_keypad = {'X', '^', 'A',
                               '<', 'v', '>'};

const point start_num{3, 2};
const point start_dir{0, 2};

class task
{
private:
    std::filesystem::path file;
    std::map<char, point> num_map;
    std::map<char, point> dir_map;
    size_t width = 3;

public:
    inline point o2p(size_t offset) const
    {
        return point{offset / width, offset % width};
    }
    inline size_t p2o(const point &pt) const
    {
        return pt.row * width + pt.col;
    }
    task(const std::filesystem::path &input) : file(input)
    {
        // init maps
        for (size_t i = 0; i < num_keypad.size(); i++)
        {
            num_map[num_keypad[i]] = o2p(i);
        }
        for (size_t i = 0; i < dir_keypad.size(); i++)
        {
            dir_map[dir_keypad[i]] = o2p(i);
        }

        std::cout << "Task input: " << file << std::endl;
    }
    std::string naive_num_path(char chcurr, char chnext)
    {
        point curr = num_map[chcurr];
        point next = num_map[chnext];
        
        dir dist = next - curr;

        // no optimising & allowing to hover 'failed' one
        auto ver = std::string(std::abs(dist.d_row), dist.d_row > 0 ? 'v' : '^');
        auto hor = std::string(std::abs(dist.d_col), dist.d_col > 0 ? '>' : '<');
        // how we throw away the unreacheable one?
        if (curr.col == 0 && next.row == 3) { // hor, ver
            return hor + ver + "A";
        }
        if (next.col == 0 && curr.row == 3) { // ver, hor
            return ver + hor + "A";
        }
        if (dist.d_col < 0) { // prioritize '<' over other
            return hor + ver + "A";
        }

        return ver + hor + "A";
    }
    std::string naive_dir_path(char chcurr, char chnext)
    {
        point curr = dir_map[chcurr];
        point next = dir_map[chnext];
        
        dir dist = next - curr;

        // no optimising & allowing to hover 'failed' one
        auto ver = std::string(std::abs(dist.d_row), dist.d_row > 0 ? 'v' : '^');
        auto hor = std::string(std::abs(dist.d_col), dist.d_col > 0 ? '>' : '<');
        // how we throw away the unreacheable one?
        if (curr.col == 0 && next.row == 0) { // hor, ver
            return hor + ver + "A";
        }
        if (next.col == 0 && curr.row == 0) { // ver, hor
            return ver + hor + "A";
        }
        if (dist.d_col < 0) { // prioritize '<' over other
            return hor + ver + "A";
        }

        return ver + hor + "A";
    }
    std::string full_dir_path(const std::string &line)
    {
        char from = 'A';
        std::string path;
        for (auto ch : line)
        {
            path += naive_dir_path(from, ch);
            from = ch;
        }
        return path;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int complexity = 0;
        while (std::getline(infile, s))
        {
            // iterating
            char from = 'A';
            std::string first_path;
            for (auto ch : s)
            {
                first_path += naive_num_path(from, ch);
                from = ch;
            }
            fmt::println("orig {}", s);
            fmt::println("1: {}", first_path);
            std::string second_path = full_dir_path(first_path);
            fmt::println("2: {}", second_path);
            std::string third_path = full_dir_path(second_path);
            fmt::println("3: {}", third_path);
            complexity += std::stoi(s)*third_path.size();
        }
        fmt::println("complexity {}", complexity);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day21" << std::endl;
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
