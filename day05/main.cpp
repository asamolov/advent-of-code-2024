#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/format.h>
#include "../utils.h"
#include <regex>
#include <map>
#include <vector>

struct update
{
    std::vector<int> pages;
    std::map<int, int> positions;
    update(const std::string &line);
    bool in_order(int left, int right) const;
    int mid_page() const;
};

struct rule
{
    int left, right;
    rule(const std::string &line);
};

template <>
struct fmt::formatter<rule> : formatter<string_view>
{

    auto format(const rule &r, format_context &ctx) const
        -> format_context::iterator;
};
auto fmt::formatter<rule>::format(const rule &r, format_context &ctx) const
    -> format_context::iterator
{
    return formatter<string_view>::format(fmt::format("{}|{}", r.left, r.right), ctx);
}

struct rules
{
    std::vector<rule> all;
    void add(const std::string &line);
    bool is_valid(const update &update);
};

rule::rule(const std::string &line)
{
    auto idx = line.find('|');
    left = std::stoi(line);
    right = std::stoi(line.substr(idx + 1));
}

class task
{
private:
    std::filesystem::path file;

public:
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    std::string run()
    {
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        rules r;
        // reading rules till empty line
        while (std::getline(infile, s) && !s.empty())
        {
            r.add(s);
        }
        fmt::print("rules:\n{}\n", fmt::join(r.all, "\n"));

        // reading updates
        while (std::getline(infile, s))
        {
            update upd(s);
            if (r.is_valid(upd))
            {
                n += upd.mid_page();
            }
        }
        return fmt::format("mid pages - {}", n);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day05" << std::endl;
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
        auto result = task.run();
        std::cout << "Result: " << result << std::endl;
    }
    else
    {
        usage(argv[0]);
    }
    return 0;
}

void rules::add(const std::string &line)
{
    all.emplace_back(line);
}

bool rules::is_valid(const update &upd)
{
    for (auto &r : all)
    {
        if (!upd.in_order(r.left, r.right))
        {
            return false;
        }
    }
    return true;
}

const std::regex digit("\\d+");

update::update(const std::string &line)
{
    auto it = std::sregex_iterator(line.begin(), line.end(), digit);
    for (decltype(it) end; it != end; ++it)
    {
        auto match = *it;
        int page = std::stoi(match.str());
        positions[page] = pages.size();
        pages.push_back(page);
    }
}

bool update::in_order(int left, int right) const
{
    if (positions.contains(left) && positions.contains(right))
    {
        return positions.at(left) < positions.at(right);
    }
    return true;
}

int update::mid_page() const
{
    return pages[pages.size() / 2];
}
