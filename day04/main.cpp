#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <regex>
#include "../utils.h"

using revsregex_iterator  = std::regex_iterator<std::string::const_reverse_iterator>;
using lines = std::vector<std::string>;

class task
{
private:
    const std::regex expr{"XMAS"};
    std::filesystem::path file;
    int find_mas_x(const lines &input) {
        int n = 0;
        int max_row = input.size();
        int max_col = input[0].size();
        for (int row = 1; row < max_row - 1; row++) { 
            for (int col = 1; col < max_col -1; col++) {
                // searching for A's
                if (input[row][col] == 'A') {
                    // checking diagonales
                    char tl = input[row-1][col-1]; // top left
                    char tr = input[row-1][col+1]; // top right
                    char bl = input[row+1][col-1]; // bottom left
                    char br = input[row+1][col+1]; // bottom right
                    if (((tl == 'M' && br == 'S')||(tl == 'S' && br == 'M'))
                    &&  ((bl == 'M' && tr == 'S')||(bl == 'S' && tr == 'M'))) {
                        n++;
                    }
                }
            }
        }
        return n;
    }
    int find_xmas(const lines &input) {
        auto end = std::sregex_iterator();
        auto revend = revsregex_iterator();
        int fwd = 0;
        int rev = 0;
        for (auto &line : input) {
            fwd += std::distance(std::sregex_iterator(line.cbegin(), line.cend(), expr), end);
            rev += std::distance(revsregex_iterator(line.crbegin(), line.crend(), expr), revend);
        }
        return fwd + rev;
    }

    lines rot90(const lines &input) { // shouldn't be transpose, but rotation
        lines result; // since input is square, we initialize result by size of first input
        result.resize(input[0].size());
        for(auto &l : input) {
            auto it = result.begin();
            for (auto ch = l.crbegin(); ch != l.crend(); ++ch, ++it) {
                it->push_back(*ch);
            }
        }
        return result;
    }

    lines rot45(const lines &input) {
        int max_row = input.size();
        int max_col = input[0].size();
        lines result;
        result.resize(max_row + max_col - 1);
        // top triangle
        auto it = result.begin();
        for (int row = 0; row < max_row; row++, ++it) {
            for (int y = row, x = 0; y >= 0 && x < max_col; y--, x++) {
                it->push_back(input[y][x]);
            }
        }
        // bottom triangle
        for (int col = 1; col < max_col; col++, ++it) {
            for (int y = max_row - 1, x = col; y >= 0 && x < max_col; y--, x++) {
                it->push_back(input[y][x]);
            }
        }
        return result;
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
        lines horizontal;
        while (std::getline(infile, s))
        {
            horizontal.push_back(std::move(s));
        }
        lines vertical = rot90(horizontal);
        lines hor45 = rot45(horizontal);
        lines ver45 = rot45(vertical);
        n += find_xmas(horizontal); // horizontal
        fmt::print("horizontal ({}):\n{}\n", find_xmas(horizontal), fmt::join(horizontal, "\n"));
        n += find_xmas(vertical); // vertical
        fmt::print("vertical ({}):\n{}\n", find_xmas(vertical), fmt::join(vertical, "\n"));
        n += find_xmas(hor45); // horisontal rotated
        fmt::print("hor45 ({}):\n{}\n", find_xmas(hor45), fmt::join(hor45, "\n"));
        n += find_xmas(ver45); // vertical rotated
        fmt::print("ver45 ({}):\n{}\n", find_xmas(ver45), fmt::join(ver45, "\n"));
        return fmt::format("n = {}, mas-x = {}", n, find_mas_x(horizontal));
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day04" << std::endl;
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
