#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"

// row, col
using point = std::pair<int, int>;
using dir = std::pair<int, int>;

class task
{
private:
    std::filesystem::path file;
    int width, height;
    std::vector<char> map;
    std::vector<bool> visited;
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
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    int descent(const point &pos, char expect) {
        if (!valid(pos)) {
            return 0;
        }
        auto offset = p2o(pos);
        auto ch = map[offset];
        if (ch != expect) {
            return 0;
        }
        if (visited[offset]) {
            return 0;
        }
        //visited[offset] = true;
        if (ch == '9') {
            return 1;
        }
        int found = 0;
        expect++;
        found += descent(move(pos, left), expect);
        found += descent(move(pos, right), expect);
        found += descent(move(pos, up), expect);
        found += descent(move(pos, down), expect);
        return found;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        width = 0;
        height = 0;
        while (std::getline(infile, s))
        {
            width = s.size();
            height++;
            for (auto ch : s) {
                map.push_back(ch);
            }
        }
        visited.resize(map.size(), false);
        auto it = map.cbegin();
        int score = 0;
        while ((it = std::find(it, map.cend(), '0')) != map.cend()) {
            std::fill(visited.begin(), visited.end(), false);
            int start = std::distance(map.cbegin(), it);
            score += descent(o2p(start), '0');
            it++;
        }
        fmt::println("score - {}", score);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day10" << std::endl;
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
