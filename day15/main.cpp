#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"
#include <set>

// row, col
using dir = aoc::dir;
using point = aoc::point<int>;

struct room
{
    const char R = '@';
    std::vector<std::string> map;
    int width, height;
    point robot;
    dir direction;
    std::vector<dir> visited;
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
    inline bool is_object(const point &pt) const
    {
        auto ch = map[pt.row][pt.col];
        return ch == 'O' || ch == '@' || ch == '[' || ch == ']';
    }
    long gps() const
    {
        long gps = 0;
        for (int row = 0; row < map.size(); row++)
        {
            for (int col = 0; col < map[row].size(); col++)
            {
                char ch = map[row][col];
                if (ch == 'O' || ch == '[')
                {
                    gps += row * 100 + col;
                }
            }
        }
        return gps;
    }
    room(const std::vector<std::string> &map) : map(map)
    {
        width = map[0].size();
        height = map.size();
        int row = 0;
        for (auto it = map.begin(); it != map.end(); ++it, row++)
        {
            auto g_pos = it->find(R);
            if (g_pos != std::string::npos)
            {
                robot = point{row, static_cast<int>(g_pos)};
                break;
            }
        }
        visited.resize(width * height); // set visited field to full map
    }
    void move(const point &old, const point &new_pt)
    {
        auto ch = map[old.row][old.col];
        map[new_pt.row][new_pt.col] = ch;
        map[old.row][old.col] = '.';
    }
    bool move(const dir &fwd)
    {
        point start = robot;
        point curr = robot;
        while (is_object(curr))
        {
            curr = fwd.move(curr);
        }
        if (can_move(curr))
        {
            dir rev = fwd.reverse();
            while (curr != start)
            {
                point next = rev.move(curr);
                move(next, curr);
                curr = next;
            }
            robot = fwd.move(robot);
            return true;
        }
        return false;
    }

    bool move2(const dir &fwd)
    {
        point start = robot;
        std::vector<point> to_move;
        std::set<point> to_check;
        std::set<point> to_check_next;
        to_check.insert(robot);
        while (!to_check.empty())
        {
            to_check_next.clear();
            for (auto curr : to_check)
            {
                char ch = map[curr.row][curr.col];
                switch (ch)
                {
                case '@':                                 // ROBOT
                case 'O':                                 // single box
                    to_move.push_back(curr);              // we need to move it
                    to_check_next.insert(fwd.move(curr)); // we need to check what's in front
                    break;
                case ']': // box end
                case '[': // box start
                    // we need to check what's in front
                    if (to_check_next.insert(fwd.move(curr)).second) // if already was checked or not
                    {
                        to_move.push_back(curr); // we need to move it
                    }
                    if (fwd == aoc::UP || fwd == aoc::DOWN)
                    {
                        // also taking care of box end
                        if (ch == '[')
                        {
                            curr.col++;
                        }
                        else
                        {
                            curr.col--;
                        }

                        // we need to check what's in front
                        if (to_check_next.insert(fwd.move(curr)).second) // if already was checked or not
                        {
                            to_move.push_back(curr); // we need to move it
                        }
                    }
                    break;
                case '#': // hit a wall
                    return false;
                case '.':
                    break;
                    // good, we just remove the element
                }
            }
            to_check.swap(to_check_next);
        }
        dir rev = fwd.reverse();
        for (auto it = to_move.crbegin(); it != to_move.crend(); ++it)
        {
            point curr = *it;
            point next = fwd.move(curr);
            move(curr, next);
        }
        robot = fwd.move(robot);
        return true;
    }
    void print()
    {
        if (height > 10)
        {
            return;
        }
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
    std::string expand(const std::string &s)
    {
        std::string expanded;
        for (auto ch : s)
        {
            switch (ch)
            {
            case '#':
            case '.':
                expanded.push_back(ch);
                expanded.push_back(ch);
                break;
            case '@':
                expanded.push_back(ch);
                expanded.push_back('.');
                break;
            case 'O':
                expanded.push_back('[');
                expanded.push_back(']');
                break;
            }
        }
        return expanded;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        std::vector<std::string> map;
        std::vector<std::string> expanded_map;
        std::string commands;
        // reading map
        while (std::getline(infile, s) && !s.empty())
        {
            map.push_back(s);
            expanded_map.push_back(expand(s));
        }
        // reading commands
        while (std::getline(infile, s))
        {
            commands += s;
        }
        fmt::println("normal map");
        room r{map};
        r.print();
        fmt::println("expanded map");
        room r2{expanded_map};
        r2.print();
        fmt::println("commands {}", commands);

        for (int i = 0; i < commands.size(); i++)
        {
            auto ch = commands.at(i);
            fmt::println("Step {}: {}", i, ch);
            r.move(dir::from_ch(ch));
            r.print();
            std::cin.get();
        }
        for (int i = 0; i < commands.size(); i++)
        {
            auto ch = commands.at(i);
            fmt::println("Step {}: {}", i, ch);
            r2.move2(dir::from_ch(ch));
            r2.print();
            std::cin.get();
        }
        // calculate gps
        fmt::println("part 1 gps - {}", r.gps());
        fmt::println("part 2 gps - {}", r2.gps());
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day00" << std::endl;
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
