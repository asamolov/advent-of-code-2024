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
const std::vector<std::tuple<bool, bool, bool, bool, bool>> sum_table = {
    {false, false, false, false, false},
    {false, true, false, true, false},
    {false, false, true, true, false},
    {false, true, true, false, true},
    {true, false, false, true, false},
    {true, true, false, false, true},
    {true, false, true, false, true},
    {true, true, true, true, true},
};

class task
{
private:
    std::filesystem::path file;
    std::map<std::string, bool> initial_wires;
    std::map<std::string, gate> gates;

public:
    task(const std::filesystem::path &input) : file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    void print_gates(size_t id)
    {
        std::string wire = fmt::format("z{:02}", id);
        std::string exp = wire;
        exp += " = ";
        exp += print_gate(wire);
        fmt::println("Gate {}", exp);
    }
    char print_op(op operation)
    {
        switch (operation)
        {
        case AND:
            return '&';
        case OR:
            return '|';
        case XOR:
            return '^';
        }
        return 'X';
    }
    std::string print_gate(const std::string &wire)
    {
        if (!gates.contains(wire))
        {
            return wire;
        }
        const auto &[left, operation, right] = gates[wire];
        std::string exp = "(";
        exp += print_gate(left);
        exp += fmt::format(" {} ", print_op(operation));
        exp += print_gate(right);
        exp += ")";
        return exp;
    }
    bool check_gate_int(const std::map<std::string, bool> &start_wires, size_t id)
    {
        // zAA should be connected to xAA and yAA
        // 1. checking that summator works
        std::map<std::string, bool> wires = start_wires;
        wires[fmt::format("x{:02}", id)] = false;
        wires[fmt::format("y{:02}", id)] = false;
        bool base_z = check_wire(wires, fmt::format("z{:02}", id));
        for (const auto [z_1, x, y, z, z1] : sum_table)
        {
            if (base_z != z_1) // checking only half of the table based on previous result
                continue;
            std::map<std::string, bool> wires = start_wires;
            wires[fmt::format("x{:02}", id)] = x;
            wires[fmt::format("y{:02}", id)] = y;
            if (z != check_wire(wires, fmt::format("z{:02}", id)) && z_1 != check_wire(wires, fmt::format("z{:02}", id + 1)))
            {
                print_gates(id);
                return false;
            }
        }
        return true;
    }
    bool check_gate(size_t id)
    {
        return check_gate_int(initial_wires, id);
    }
    bool check_gate_neg(size_t id)
    {
        // negating main check
        // zAA should be connected to xAA and yAA
        // 1. checking that summator works
        std::map<std::string, bool> wires = initial_wires;
        // negating all wires
        for (auto &wire : wires)
        {
            wire.second = !wire.second;
        }

        return check_gate_int(initial_wires, id);
    }

    bool check_wire(std::map<std::string, bool> &wires, const std::string &wire)
    {
        if (wires.contains(wire))
        {
            return wires[wire];
        }
        if (!gates.contains(wire))
        {
            return false;
        }
        const auto &[left, operation, right] = gates[wire];
        auto w_left = check_wire(wires, left);
        auto w_right = check_wire(wires, right);
        bool val;
        switch (operation)
        {
        case AND:
            val = w_left & w_right;
            break;
        case OR:
            val = w_left | w_right;
            break;
        case XOR:
            val = w_left ^ w_right;
            break;
        }
        wires[wire] = val;
        return val;
    }
    void validate()
    {
        for (size_t i = 0; i < 45; i++)
        {
            if (!check_gate(i))
            {
                fmt::println("wire z{:02} is misconnected", i);
            }
            else if (!check_gate_neg(i))
            {
                fmt::println("NEG wire z{:02} is misconnected", i);
            }
        }
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
                initial_wires[match[1]] = match[2] != "0";
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
        std::map<std::string, bool> wires(initial_wires);
        for (size_t i = 0; i < 64; i++)
        {
            result |= static_cast<uint64_t>(check_wire(wires, fmt::format("z{:02}", i))) << i;
        }

        fmt::println("result {}", result);
        fmt::println("result {:#064b}", result);

        for (size_t i = 0; i < 12; i++)
        {
            print_gates(i);
        }
        validate();
        fmt::println("all zeroes wires");
        for (size_t i = 0; i < 45; i++)
        {
            initial_wires[fmt::format("x{:02}", i)] = false;
            initial_wires[fmt::format("y{:02}", i)] = false;
        }

        validate();
        fmt::println("all ones wires");
        for (size_t i = 0; i < 45; i++)
        {
            initial_wires[fmt::format("x{:02}", i)] = true;
            initial_wires[fmt::format("y{:02}", i)] = true;
        }

        validate();
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
