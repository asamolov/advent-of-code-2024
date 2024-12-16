#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"
#include <queue>
#include <map>

using dir = aoc::dir;
using point = aoc::point<int>;

struct room
{
    const char S = 'S';
    std::vector<std::string> map;
    int width, height;
    point deer;
    dir direction;
    std::vector<int> visited;
    bool found_loop;
    bool mark_path;
    inline point o2p(int offset) const
    {
        return point{offset / width, offset % width};
    }
    inline int p2o(const point &pt) const
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
        width = map[0].size();
        height = map.size();
        int row = 0;
        for (auto it = map.begin(); it != map.end(); ++it, row++)
        {
            auto g_pos = it->find('S');
            if (g_pos != std::string::npos)
            {
                deer = point{row, static_cast<int>(g_pos)};
                break;
            }
        }
        visited.resize(width * height); // set visited field to full map
        direction = aoc::RIGHT;
    }
    void move(const point &old, const point &new_pt)
    {
        auto ch = map[old.row][old.col];
        map[new_pt.row][new_pt.col] = ch;
        map[old.row][old.col] = '.';
    }
    inline char at(const point &pt) const
    {
        return map[pt.row][pt.col];
    }
    void visit(const point &pt, int distance, const dir &direction)
    {
        visited[p2o(pt)] = distance;
        map[pt.row][pt.col] = direction;
    }
    inline bool has_visited_faster(const point &pt, int dist) const
    {
        return visited[p2o(pt)] >= 0 && visited[p2o(pt)] <= dist;
    }
    int shortest_path()
    {
        std::fill(visited.begin(), visited.end(), -1);
        using tpl = std::tuple<int, point, point, dir>;
        std::map<std::pair<point, point>, int> passed_edges; 
        std::priority_queue<tpl, std::vector<tpl>, std::greater<tpl>> to_visit;
        to_visit.push({0, deer, deer, direction});
        int step = 0;
        while (!to_visit.empty())
        {
            if (step % 1000 == 0) {
                fmt::println("Step {}...", step);
                print();
            }
            step++;
            auto curr = to_visit.top();
            to_visit.pop();
            auto [dist, prev_pt, pt, curr_dir] = curr;
            auto key = std::pair{prev_pt, pt};
            if (passed_edges.contains(key)) {
                if (passed_edges[key] <= dist) {
                    continue;
                }
            }
            switch (at(pt))
            {
            case 'E':
                fmt::println("Reached end at step {}...", step);
                print();
                return dist;
            case '#':
                continue;
            case 'S':
            case '.':
            case '>':
            case '<':
            case '^':
            case 'v':
                // go forward
                visit(pt, dist, curr_dir);
                passed_edges[key] = dist;
                to_visit.emplace(dist + 1, pt, curr_dir.move(pt), curr_dir);
                to_visit.emplace(dist + 1001, pt, curr_dir.rot90cw().move(pt), curr_dir.rot90cw());
                to_visit.emplace(dist + 1001, pt, curr_dir.rot90ccw().move(pt), curr_dir.rot90ccw());
                break;
            };
        };

        return -1;
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
        //std::cin.get();
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
        std::string commands;
        // reading map
        while (std::getline(infile, s))
        {
            map.push_back(s);
        }
        room r{map};
        fmt::println("path - {}", r.shortest_path());
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day16" << std::endl;
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
