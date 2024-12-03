#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <regex>
#include <fmt/core.h>
#include "../utils.h"

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
        std::regex expr("mul\\((\\d+),(\\d+)\\)|don't\\(\\)|do\\(\\)");
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        int conditional = 0;
        bool enabled = true;
        while (std::getline(infile, s))
        {
            auto it = std::sregex_iterator(s.begin(), s.end(), expr);
            for (decltype(it) end; it != end; ++it)
            {
                auto match = *it;
                if ("don't()" == match.str()) { enabled = false; }
                else if ("do()" == match.str()) { enabled = true; }
                else
                {
                    int mul = std::stoi(match[1]) * std::stoi(match[2]);
                    n += mul;
                    conditional += enabled * mul;
                }
            }
        }
        return fmt::format("sum of mul's - {}, conditional - {}", n, conditional);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day03" << std::endl;
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
