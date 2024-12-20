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
    std::vector<int> _visited;
    std::map<point, point> path;
    point start, finish;

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
    room(const std::vector<std::string> &map) : map(map)
    {
        height = map.size();
        width = map[0].size();
        size_t row = 0;
        for (auto it = map.begin(); it != map.end(); ++it, row++)
        {
            auto pos = it->find('S');
            if (pos != std::string::npos)
            {
                start = point{row, pos};
                break;
            }
            else
            {
                pos = it->find('E');
                if (pos != std::string::npos)
                {
                    finish = point{row, pos};
                }
            }
        }
        _visited.resize(width * height, -1);
    }
    inline char at(const point &pt) const
    {
        return map[pt.row][pt.col];
    }
    inline int cheat(const point &pt, const dir &from, const dir &to)
    {
        // can cheat?
        if (_visited[p2o(pt)] != -1 || from == to)
        {
            return 0;
        }
        auto from_dist = _visited[p2o(from.move(pt))];
        auto to_dist = _visited[p2o(to.move(pt))];
        if (from_dist == -1 || to_dist == -1)
        {
            return 0;
        }
        else
        {
            return std::max(0, to_dist - from_dist - 2);
        }
    }
    void visit(const point &pt, int distance)
    {
        _visited[p2o(pt)] = distance;
        map[pt.row][pt.col] = ' ';
    }
    bool visited(const point &pt) const
    {
        return _visited[p2o(pt)] >= 0;
    }
    bool can_go(const point &pt) const
    {
        return map[pt.row][pt.col] != '#';
    }
    auto find_path()
    {
        std::fill(_visited.begin(), _visited.end(), -1);
        std::queue<std::pair<point, size_t>> frontier;
        path.clear();
        frontier.push({start, 0});
        visit(start, 0);
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
                    visit(next, distance + 1);
                    // path[next] = pt;
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
        std::vector<std::string> map;
        std::map<int, int> gains;
        while (std::getline(infile, s))
        {
            map.push_back(s);
        }
        room r{map};
        auto full_path = r.find_path();
        for (size_t i = 1; i < r.height - 1; i++)
        {
            for (size_t j = 1; j < r.width - 1; j++)
            {
                for (dir from : aoc::ALL_DIRS)
                {
                    for (dir to : aoc::ALL_DIRS)
                    {
                        gains[r.cheat(point{i, j}, from, to)]++;
                    }
                }
            }
        }

        int major_gains = 0;
        int total_gains = 0;
        for (auto [gain, count] : gains)
        {
            fmt::println("Gained {}: {} times", gain, count);
            if (gain >= 100)
            {
                major_gains += count;
            }
            total_gains += count;
        }
        fmt::println("full_path - {}, all_gains - {}, major gains - {}", full_path, total_gains - gains[0], major_gains);
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
