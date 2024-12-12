#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"
#include <map>
#include <vector>
#include <algorithm>

// row, col
using point = std::pair<int, int>;
using dir = std::pair<int, int>;
using borders = std::map<std::pair<int, bool>, std::vector<int>>;

struct region
{
    int area;
    int perimeter;
    int sides;
};

class task
{
private:
    std::filesystem::path file;
    int width, height;
    std::vector<char> map;
    std::vector<bool> visited;
    std::map<std::pair<char,int>, region> regions;
    std::map<std::pair<char,int>, borders> allborders;

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
    bool flood(const point &pos, char expect, region &r, borders &b)
    {
        if (!valid(pos))
        {
            // hit the border of the map
            r.perimeter++;
            return true;
        }
        auto offset = p2o(pos);
        auto ch = map[offset];
        if (ch != expect)
        {
            // hit the border with other region
            r.perimeter++;
            return true;
        }
        if (visited[offset])
        {
            // been there
            return false;
        }
        visited[offset] = true;
        r.area++;
        point pt = move(pos, left);
        if (flood(pt, expect, r, b)) {
            b[std::pair{pos.second * 2 - 1, true}].push_back(-pt.first);
        }
        pt = move(pos, right);
        if (flood(pt, expect, r, b)) {
            b[std::pair{pos.second * 2 + 1, true}].push_back(pt.first);
        }
        pt = move(pos, up);
        if (flood(pt, expect, r, b)) {
            b[std::pair{pos.first * 2 - 1, false}].push_back(-pt.second);
        }
        pt = move(pos, down);
        if (flood(pt, expect, r, b)) {
            b[std::pair{pos.first * 2 + 1, false}].push_back(pt.second);
        }
        return false;
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
            std::pair<char, int> r{ch, start};
            auto &reg = regions[r];
            auto &brdr = allborders[r];
            flood(o2p(start), ch, reg, brdr);
            // count sides
            for (auto &b : brdr) {
                // looking for continious border
                auto &line = b.second;
                std::sort(line.begin(), line.end());
                int prev = line.front();
                for (auto p : line) {
                    prev++;
                    if (p != prev) {
                        // new segment
                        reg.sides++;
                        prev = p;
                    }
                }
            }
        }
        long price = 0;
        long discounted = 0;
        for (auto &r : regions) {
            price += r.second.area * r.second.perimeter;
            discounted += r.second.area * r.second.sides;
        }
        fmt::println("price - {}, discounted - {}", price, discounted);
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
