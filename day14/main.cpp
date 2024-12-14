#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <regex>
#include "../utils.h"

const int width = 101;
const int height = 103;
// const int width = 11;
// const int height = 7;

using position = std::pair<int, int>;
using velocity = std::pair<int, int>;

const std::regex r_line("p=(\\d+),(\\d+) v=(-?\\d+),(-?\\d+)");

struct robot
{
    position pos;
    velocity v;

    robot(const std::string &s)
    {
        // p=0,4 v=3,-3
        std::smatch match;
        std::regex_match(s, match, r_line);
        pos = position{std::stoi(match[1]), std::stoi(match[2])};
        v = velocity{std::stoi(match[3]), std::stoi(match[4])};
    }

    void move(int time = 1)
    {
        pos.first = (time * v.first + pos.first) % width;
        if (pos.first < 0)
        {
            pos.first += width;
        }
        pos.second = (time * v.second + pos.second) % height;
        if (pos.second < 0)
        {
            pos.second += height;
        }
    }

    int quadrant()
    {
        if (pos.first == width / 2 || pos.second == height / 2)
        {
            return 0;
        }
        bool left = pos.first < width / 2;
        bool top = pos.second < height / 2;
        if (left)
        {
            if (top)
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
        else
        {
            if (top)
            {
                return 3;
            }
            else
            {
                return 4;
            }
        }
    }
};

class task
{
private:
    std::filesystem::path file;

public:
    inline bool valid(int x, int y) const
    {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
    int flood(std::vector<std::string> &field, int x, int y)
    {
        std::vector<position> stack;
        stack.emplace_back(x, y);
        stack.emplace_back(x + 1, y + 0);
        stack.emplace_back(x - 1, y - 0);
        stack.emplace_back(x + 0, y + 1);
        stack.emplace_back(x - 0, y - 1);
        int n = 0; // how many points we colored
        do
        {
            auto pos = stack.back();
            x = pos.first;
            y = pos.second;
            stack.pop_back();
            if (!valid(x, y))
            {
                continue;
            }
            if (field[y][x] != '.')
            {
                // hit the robot or already visited
                continue;
            }
            field[y][x] = ' ';
            n++;
            stack.emplace_back(x + 1, y + 0);
            stack.emplace_back(x - 1, y - 0);
            stack.emplace_back(x + 0, y + 1);
            stack.emplace_back(x - 0, y - 1);
        } while (!stack.empty());
        return n;
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
        int n = 0;
        std::vector<int> quadrants;
        quadrants.resize(5, 0);
        std::vector<robot> robots;
        while (std::getline(infile, s))
        {
            robot r(s);
            robots.push_back(r);
            r.move(100);
            quadrants[r.quadrant()]++;
        }

        std::vector<std::string> field;
        for (int i = 0; i < 10000000; i++)
        {
            field.clear();
            field.resize(height, std::string(width, '.'));
            
            //fmt::println("Step {} {}", i, std::string(width, '='));
            for (auto &r : robots)
            {
                field[r.pos.second][r.pos.first] = '*';
                r.move();
            }
            int flood_factor = flood(field, width / 2, height / 2);
            if (flood_factor < width * height / 2)
            {
                fmt::println("Step {} {}", i, std::string(width, '='));
                for (auto &l : field)
                {
                    fmt::println("{}", l);
                }
                std::cin.get();
                break;
            }
            //
        }

        int factor = quadrants[1] * quadrants[2] * quadrants[3] * quadrants[4];
        fmt::println("safety factor - {}", factor);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day14" << std::endl;
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
