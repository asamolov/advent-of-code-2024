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
    inline int check_distance(const point &from, const point &to, int cheat_size)
    {
        if (!valid(to))
        {
            return 0;
        }
        auto from_dist = _visited[p2o(from)];
        auto to_dist = _visited[p2o(to)];
        if (to_dist == -1)
        {
            return 0;
        }
        return std::max(0, to_dist - from_dist - cheat_size);
    }
    inline int cheat_in_radius(std::map<int, uint64_t> &cheats, const point &pt, unsigned int radius)
    {
        // can start cheating?
        if (_visited[p2o(pt)] == -1)
        {
            return 0;
        }
        // looking for point with distance more than radius
        for (int i = 1; i <= radius; i++)
        {
            // i + j <= radius
            for (int j = 1; j <= radius - i; j++)
            {
                // checking +-i, +-j
                cheats[check_distance(pt, dir{i, j}.move(pt), i + j)]++;
                cheats[check_distance(pt, dir{-i, j}.move(pt), i + j)]++;
                cheats[check_distance(pt, dir{i, -j}.move(pt), i + j)]++;
                cheats[check_distance(pt, dir{-i, -j}.move(pt), i + j)]++;
            }
            // axis are treated separately
            cheats[check_distance(pt, dir{i, 0}.move(pt), i)]++;
            cheats[check_distance(pt, dir{-i, 0}.move(pt), i)]++;
            cheats[check_distance(pt, dir{0, i}.move(pt), i)]++;
            cheats[check_distance(pt, dir{0, -i}.move(pt), i)]++;
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
        frontier.push({start, 0});
        visit(start, 0);
        for (; !frontier.empty(); frontier.pop())
        {
            auto [pt, distance] = frontier.front();
            if (pt == finish)
            {
                return distance;
            }
            for (dir d : aoc::ALL_DIRS)
            {
                auto next = d.move(pt);
                if (valid(next) && !visited(next) && can_go(next))
                {
                    visit(next, distance + 1);
                    frontier.emplace(next, distance + 1);
                }
            }
        }
        return std::numeric_limits<size_t>::max();
    }
    void print()
    {
        for (auto &r : map)
        {
            fmt::println("{}", r);
        }
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
        // both part 1 & part 2 can be solved with the same approach
        // cheating in radius 2 and 20 cells.
        std::map<int, uint64_t> huge_cheats;
        std::map<int, uint64_t> small_cheats;
        for (size_t i = 1; i < r.height - 1; i++)
        {
            for (size_t j = 1; j < r.width - 1; j++)
            {
                r.cheat_in_radius(small_cheats, point{i, j}, 2);
                r.cheat_in_radius(huge_cheats, point{i, j}, 20);
            }
        }

        uint64_t major_gains = 0;
        uint64_t total_gains = 0;
        for (auto [gain, count] : small_cheats)
        {
            // fmt::println("Gained {}: {} times", gain, count);
            if (gain >= 100)
            {
                major_gains += count;
            }
            total_gains += count;
        }
        uint64_t huge_gains = 0;
        for (auto [gain, count] : huge_cheats)
        {
            if (gain >= 100)
            {
                fmt::println("Radius gained {}: {} times", gain, count);
                huge_gains += count;
            }
        }
        fmt::println("full_path - {}, all_gains - {}, major gains - {}, huge gains - {}", full_path, total_gains - gains[0], major_gains, huge_gains);
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
