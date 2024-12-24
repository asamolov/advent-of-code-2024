#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include <map>
#include <regex>

enum op : uint8_t
{
    AND,
    OR,
    XOR
};

using gate = std::tuple<std::string, op, std::string>;

const std::regex wire_exp(R"((\w\d\d): (\d))");
const std::regex gate_exp(R"((\w+) (\w+) (\w+) -> (\w+))");

class task
{
private:
    std::filesystem::path file;
    std::map<std::string, bool> wires;
    std::map<std::string, gate> gates;

public:
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }

    bool check_wire(const std::string &wire) {
        if (wires.contains(wire)) {
            return wires[wire];
        }
        if (!gates.contains(wire)) {
            return false;
        }
        const auto &[left, operation, right] = gates[wire];
        auto w_left = check_wire(left);
        auto w_right = check_wire(right);
        bool val;
        switch (operation) {
            case AND:
            val = w_left & w_right;
            break;
            case OR:
            val =  w_left | w_right;
            break;
            case XOR:
            val = w_left ^ w_right;
            break;
        }
        wires[wire] = val;
        return val;
    }
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        while (std::getline(infile, s) && !s.empty())
        {
            std::smatch match;
            if (std::regex_match(s, match, wire_exp))
            {
                wires[match[1]] = match[2] != "0";
            }
        }
        while (std::getline(infile, s) && !s.empty())
        {
            std::smatch match;
            if (std::regex_match(s, match, gate_exp))
            {
                op operation;
                if (match[2] == "AND")
                {
                    operation = AND;
                }
                else if (match[2] == "OR")
                {
                    operation = OR;
                }
                else
                {
                    operation = XOR;
                }
                gates[match[4]] = gate{match[1], operation, match[3]};
            }
        }
        uint64_t result = 0;
        for (size_t i = 0; i < 64; i++)
        {
            result |= static_cast<uint64_t>(check_wire(fmt::format("z{:02}", i))) << i;
        }

        fmt::println("result {}", result);
        fmt::println("result {:#064b}", result);
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day24" << std::endl;
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
