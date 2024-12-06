#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include <set>
#include <map>
#include <unordered_map>

// row, col
using point = std::pair<int, int>;
using dir = std::pair<int, int>;

template<>
struct std::hash<point>
{
    std::size_t operator()(const point& s) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(s.first);
        std::size_t h2 = std::hash<int>{}(s.second);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};

struct room
{
    const char G = '^';
    std::vector<std::string> map;
    int width, height;
    point guard, orig_guard;
    dir direction;
    std::unordered_map<point, dir> visited;
    bool found_loop;
    bool mark_path;
    inline point move(const point &pt, const dir &direction)
    {
        return point{pt.first + direction.first, pt.second + direction.second};
    }
    inline void rot()
    {
        // up: -1, 0; down 1, 0; left 0, -1; right 0, 1
        // 90 cw rotation is achieved by multiplying direction vector by rotation matrix
        // [ 0  1 ] [ x ]
        // [ -1 0 ] [ y ]
        direction = dir{direction.second, -direction.first};
    }
    void reset() {
        direction = {-1, 0};
        guard = orig_guard;
        visited.clear();
        visited[guard] = direction;
        found_loop = false;
    }
    room(const std::vector<std::string> &map, bool mark_path = true) : map(map), mark_path(mark_path)
    {
        width = map[0].size();
        height = map.size();
        int row = 0;
        for (auto it = map.begin(); it != map.end(); ++it, row++)
        {
            auto g_pos = it->find(G);
            if (g_pos != std::string::npos)
            {
                orig_guard = point{row, g_pos};
                break;
            }
        }
        reset();
    }
    // false once the guard is about to leave the room
    bool move()
    {
        point new_pos;
        do
        {
            // can move?
            new_pos = move(guard, direction);
            // out of map?
            if (new_pos.first < 0 || new_pos.first >= height || new_pos.second < 0 || new_pos.second >= width)
            {
                return false;
            }
            if (map[new_pos.first][new_pos.second] != '#')
            {
                // check for loops
                if (visited.contains(new_pos)) {
                    if (visited.at(new_pos) == direction) {
                        found_loop = true;
                        return false;
                    }
                } else {
                    visited[new_pos] = direction;
                }
                break;
            }
            // hit obstacle
            rot();
        } while (true);
        if (mark_path) {
            map[guard.first][guard.second] = 'X';
            map[new_pos.first][new_pos.second] = '^';
        }
        guard = new_pos;
        return true;
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
        while (std::getline(infile, s))
        {
            map.push_back(s);
        }
        room r{map};
        point guard{r.guard};
        int step = 0;
        do
        {
            //fmt::println("step {}\n{}", step, fmt::join(r.map, "\n"));
            //fmt::println("step {}", step);
            step++;
        } while (r.move());
        timer.stop("First run");
        std::unordered_map<point, dir> to_review{r.visited};
        to_review.erase(guard);
        fmt::println("reviewing {} positions to place and obstacle", to_review.size());
        int obstacles = 0;
        int checked = 0;
        room fixed{map, false};
        for (auto pt : to_review) {
            //fmt::println("Checking {} of {} obstacle place...", ++checked, to_review.size());
            fixed.map[pt.first.first][pt.first.second] = '#'; // placing obstacle
            while (fixed.move());
            obstacles += fixed.found_loop;
            fixed.map[pt.first.first][pt.first.second] = '.'; // removing obstacle
            fixed.reset(); // resetting to avoid recalculating
        }
        fmt::println("visited {}, places for obstacles {}", r.visited.size(), obstacles);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day06" << std::endl;
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
