#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"
#include <map>

// row, col
using point = std::pair<int, int>;
using dir = std::pair<int, int>;

struct region
{
    int area;
    int perimeter;
};

class task
{
private:
    std::filesystem::path file;
    int width, height;
    std::vector<char> map;
    std::vector<bool> visited;
    std::map<std::pair<char,int>, region> regions;

public:
    // up: -1, 0; down 1, 0; left 0, -1; right 0, 1
    const dir left{0, -1};
    const dir right{0, 1};
    const dir up{-1, 0};
    const dir down{1, 0};
    inline point o2p(int offset) const
    {
        return point{offset / width, offset % width};
    }
    inline int p2o(const point &pt) const
    {
        return pt.first * width + pt.second;
    }
    inline point move(const point &pt, const dir &direction)
    {
        return point{pt.first + direction.first, pt.second + direction.second};
    }
    inline bool valid(const point &pt) const
    {
        return pt.first >= 0 && pt.first < height && pt.second >= 0 && pt.second < width;
    }
    int flood(const point &pos, char expect, region &r)
    {
        if (!valid(pos))
        {
            // hit the border of the map
            r.perimeter++;
            return 0;
        }
        auto offset = p2o(pos);
        auto ch = map[offset];
        if (ch != expect)
        {
            // hit the border with other region
            r.perimeter++;
            return 0;
        }
        if (visited[offset])
        {
            // been there
            return 0;
        }
        visited[offset] = true;
        r.area++;
        int found = 0;
        found += flood(move(pos, left), expect, r);
        found += flood(move(pos, right), expect, r);
        found += flood(move(pos, up), expect, r);
        found += flood(move(pos, down), expect, r);
        return found;
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
        width = 0;
        height = 0;
        while (std::getline(infile, s))
        {
            width = s.size();
            height++;
            for (auto ch : s)
            {
                map.push_back(ch);
            }
        }
        visited.resize(map.size(), false);
        for (auto it = visited.cbegin(); it != visited.cend(); it++)
        {
            if (*it) {
                continue;
            }
            int start = std::distance(visited.cbegin(), it);
            char ch = map[start];
            flood(o2p(start), ch, regions[std::pair{ch, start}]);
        }
        long price = 0;
        for (auto &r : regions) {
            price += r.second.area * r.second.perimeter;
        }
        fmt::println("price - {}", price);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day12" << std::endl;
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
