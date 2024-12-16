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
        auto &d = visited[p2o(pt)];
        if (distance < d)
        {
            d = distance;
            map[pt.row][pt.col] = direction;
        }
    }
    inline bool has_visited_faster(const point &pt, int dist) const
    {
        return visited[p2o(pt)] >= 0 && visited[p2o(pt)] <= dist;
    }
    std::pair<int, int> shortest_path()
    {
        std::fill(visited.begin(), visited.end(), std::numeric_limits<int>::max());
        using tpl = std::tuple<int, point, point, dir>;
        std::map<std::pair<point, point>, int> passed_edges;
        std::priority_queue<tpl, std::vector<tpl>, std::greater<tpl>> to_visit;
        to_visit.push({0, deer, deer, direction});
        int step = 0;
        point end;
        int min_dist = std::numeric_limits<int>::max();
        while (!to_visit.empty())
        {
            if (step % 1000 == 0)
            {
                fmt::println("Step {}...", step);
                print();
            }
            step++;
            auto curr = to_visit.top();
            to_visit.pop();
            auto [dist, prev_pt, pt, curr_dir] = curr;
            if (dist > min_dist)
            {
                continue;
            }
            auto key = std::pair{prev_pt, pt};
            if (passed_edges.contains(key))
            {
                if (passed_edges[key] <= dist)
                {
                    continue;
                }
            }
            switch (at(pt))
            {
            case 'E':
                fmt::println("Reached end at step {}...", step);
                print();
                end = pt;
                min_dist = std::min(min_dist, dist);
                passed_edges[key] = dist;
                continue;
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
        }

        // deconstructing best paths

        std::map<point, int> path_nodes;
        path_nodes[end] = min_dist;
        point curr = end;
        int dist = min_dist;
        std::priority_queue<tpl, std::vector<tpl>, std::greater<tpl>> to_visit_back;
        to_visit_back.emplace(dist, curr, aoc::UP.move(curr), aoc::UP);
        to_visit_back.emplace(dist, curr, aoc::DOWN.move(curr), aoc::DOWN);
        to_visit_back.emplace(dist, curr, aoc::LEFT.move(curr), aoc::LEFT);
        to_visit_back.emplace(dist, curr, aoc::RIGHT.move(curr), aoc::RIGHT);

        while (!to_visit_back.empty())
        {
            auto curr = to_visit_back.top();
            to_visit_back.pop();
            auto [dist, prev_pt, pt, curr_dir] = curr;
            auto key = std::pair{pt, prev_pt};

            if (passed_edges.contains(key))
            {
                if (passed_edges[key] == dist)
                {
                    path_nodes[pt] = dist;
                    to_visit_back.emplace(dist - 1, pt, curr_dir.move(pt), curr_dir);
                    to_visit_back.emplace(dist - 1001, pt, curr_dir.move(pt), curr_dir);
                    to_visit_back.emplace(dist - 1, pt, curr_dir.rot90cw().move(pt), curr_dir.rot90cw());
                    to_visit_back.emplace(dist - 1001, pt, curr_dir.rot90cw().move(pt), curr_dir.rot90cw());
                    to_visit_back.emplace(dist - 1, pt, curr_dir.rot90ccw().move(pt), curr_dir.rot90ccw());
                    to_visit_back.emplace(dist - 1001, pt, curr_dir.rot90ccw().move(pt), curr_dir.rot90ccw());
                }
            }
        }

        return {min_dist, path_nodes.size()};
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
        std::string commands;
        // reading map
        while (std::getline(infile, s))
        {
            map.push_back(s);
        }
        room r{map};
        auto result = r.shortest_path();
        fmt::println("path - {}, best places - {}", result.first, result.second);
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
