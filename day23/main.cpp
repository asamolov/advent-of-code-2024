#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include "../utils.h"

#include <regex>
#include <map>
#include <set>
#include <array>
#include <algorithm>

const std::regex conn_exp{"(\\w+)-(\\w+)"};

using lan = std::array<std::string, 3>;

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
        std::map<std::string, std::set<std::string>> conns;
        int n = 0;
        while (std::getline(infile, s))
        {
            std::smatch match;
            if (std::regex_match(s, match, conn_exp))
            {
                conns[match[1]].insert(match[2]);
                conns[match[2]].insert(match[1]);
            }
        }

        std::set<lan> lans;
        for (auto &conn : conns)
        {
            auto &a = conn.first;
            // a <-> b, looking for intersection of a <-> c, b <-> c
            auto &ac = conn.second;
            for (auto &b : conn.second)
            {
                auto &bc = conns[b];

                std::vector<std::string> all_c;
                // intesect
                std::set_intersection(ac.begin(), ac.end(), bc.begin(), bc.end(), std::back_inserter(all_c));
                for (auto &c : all_c)
                {
                    if (a.front() == 't' || b.front() == 't' || c.front() == 't')
                    {
                        lan l{a, b, c};
                        std::sort(l.begin(), l.end());
                        lans.insert(l);
                    }
                }
            }
        }
        fmt::println("lans - {}", lans.size());
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day23" << std::endl;
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
