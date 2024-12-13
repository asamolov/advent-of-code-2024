#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"
#include <regex>

const std::regex button("Button \\w: X+(\\d+), Y+(\\d+)");
const std::regex prize("Prize: X=(\\d+), Y=(\\d+)");
const std::regex coords(".+X.(\\d+), Y.(\\d+)");

using point = std::pair<long long, long long>;

class task
{
private:
    std::filesystem::path file;
public:
    point take(const std::string &s) {
        std::smatch match;
        std::regex_match(s, match, coords);
        return point{std::stoi(match[1]), std::stoi(match[2])};
    }
    long long solve(point first, point second, point result) {
        long long det = first.first * second.second - first.second * second.first;
        if (det == 0) {
            fmt::println("Det = 0!");
            return 0;
        }
        long long det1 = result.first * second.second - result.second * second.first;
        long long det2 = first.first * result.second - first.second * result.first;
        long long a = det1 / det;
        long long b = det2 / det;
        if (det1 % det == 0 && det2 % det == 0 && a >= 0 && b >= 0 /*&& a <= 100  && b <= 100*/) {
        return 3*a + b;

        }
        return 0;
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
        std::vector<point> pts;
        long long n = 0;
        while (std::getline(infile, s))
        {
            point first = take(s);
            std::getline(infile, s);
            point second = take(s);
            std::getline(infile, s);
            point result = take(s);
            result.first += 10000000000000;
            result.second += 10000000000000;
            std::getline(infile, s); // empty
            n+= solve(first, second, result);
        }
        fmt::println("n chars - {}", n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day13" << std::endl;
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
