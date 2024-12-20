#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"

#include <queue>
#include <regex>
#include <map>

using dir = aoc::dir;
using point = aoc::point<size_t>;

struct room
{
    std::vector<std::string> map;
    size_t width, height;
    std::vector<bool> _visited;
    std::map<point, point> path;

    inline point o2p(size_t offset) const
    {
        return point{offset / width, offset % width};
    }
    inline size_t p2o(const point &pt) const
    {
        return pt.row * width + pt.col;
    }
    inline bool valid(const point &pt) const
    {
        return pt.row >= 0 && pt.row < height && pt.col >= 0 && pt.col < width;
    }
    inline bool can_move(const point &pt) const
    {
        return map[pt.row][pt.col] == '.';
    }
    room(size_t width, size_t height) : width(width), height(height)
    {
        map.resize(height, std::string(width, '.'));
        _visited.resize(width * height, false);
    }
    inline char at(const point &pt) const
    {
        return map[pt.row][pt.col];
    }
    inline bool corrupt(const point &pt)
    {
        map[pt.row][pt.col] = '#';
        //return path.contains(pt);
        return true;
    }
    void visit(const point &pt)
    {
        _visited[p2o(pt)] = true;
        map[pt.row][pt.col] = ' ';
    }
    bool visited(const point &pt) const
    {
        return _visited[p2o(pt)];
    }
    bool can_go(const point &pt) const
    {
        return map[pt.row][pt.col] != '#';
    }
    auto find_path()
    {
        point start{0, 0};
        point finish{height - 1, width - 1};
        std::fill(_visited.begin(), _visited.end(), false);
        std::queue<std::pair<point, size_t>> frontier;
        path.clear();
        frontier.push({start, 0});
        visit(start);
        for (; !frontier.empty(); frontier.pop())
        {
            auto [pt, distance] = frontier.front();
            if (pt == finish)
            {
                // mark path
                // point head = pt;
                // map[head.row][head.col] = 'O';

                // while (parent.contains(head))
                // {
                //     head = parent[head];
                //     map[head.row][head.col] = 'O';
                // }
                // fmt::println("Found path from start to finish");
                // print();
                return distance;
            }
            for (dir d : aoc::ALL_DIRS)
            {
                auto next = d.move(pt);
                if (valid(next) && !visited(next) && can_go(next))
                {
                    visit(next);
                    //path[next] = pt;
                    frontier.emplace(next, distance + 1);
                }
            }
        }
        return std::numeric_limits<size_t>::max();
    }
    void print()
    {
        // if (height > 10)
        // {
        //     return;
        // }
        for (auto &r : map)
        {
            fmt::println("{}", r);
        }
        // std::cin.get();
    }
};


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
        int n = 0;
        while (std::getline(infile, s))
        {
            n += s.size();
        }
        fmt::println("n chars - {}", n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day20" << std::endl;
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
