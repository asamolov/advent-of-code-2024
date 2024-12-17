#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "../utils.h"
#include <functional>
#include <regex>

const std::regex register_line("Register \\w: (\\d+)");
const std::regex program_line("Program: (\\d(,\\d)*)");

// see instruction definitions in https://adventofcode.com/2024/day/17
enum op : uint8_t
{
    adv = 0b000,
    bxl,
    bst,
    jnz,
    bxc,
    out,
    bdv,
    cdv
};

using reg = uint64_t;

struct regfile
{
    reg a;
    reg b;
    reg c;

    void print()
    {
        fmt::println("Register A: {}", a);
        fmt::println("Register B: {}", b);
        fmt::println("Register C: {}", c);
    }
};

struct computer
{
    std::vector<uint8_t> instructions;
    size_t ip;
    regfile regs;
    std::vector<uint8_t> output;
    std::vector<std::function<void(uint8_t)>> i_map;
    computer(reg a, reg b, reg c, std::string &s)
    {
        regs.a = a;
        regs.b = b;
        regs.c = c;
        for (auto ch : s)
        {
            if (ch == ',') {
                continue;
            }
            instructions.push_back((ch - '0') & 0b111);
        }
        ip = 0;

        using std::placeholders::_1;
        i_map.push_back(std::bind(&computer::adv, this, _1));
        i_map.push_back(std::bind(&computer::bxl, this, _1));
        i_map.push_back(std::bind(&computer::bst, this, _1));
        i_map.push_back(std::bind(&computer::jnz, this, _1));
        i_map.push_back(std::bind(&computer::bxc, this, _1));
        i_map.push_back(std::bind(&computer::out, this, _1));
        i_map.push_back(std::bind(&computer::bdv, this, _1));
        i_map.push_back(std::bind(&computer::cdv, this, _1));
    }

    void print()
    {
        fmt::println("IP: {}", ip);
        regs.print();
        fmt::println("OUT: {}", fmt::join(output, ","));
        fmt::println("{}", std::string{10, '='});
    }

    reg decode(uint8_t op)
    {
        switch (op)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            return op;
        case 4:
            return regs.a;
        case 5:
            return regs.b;
        case 6:
            return regs.c;
        }
        return 0xFF;
    }

    void adv(uint8_t combo)
    {
        regs.a = regs.a >> decode(combo);
    }
    void bxl(uint8_t op)
    {
        regs.b ^= op;
    }
    void bst(uint8_t combo)
    {
        regs.b = decode(combo) & 0b111;
    }
    void jnz(uint8_t op)
    {
        if (regs.a != 0)
        {
            ip = op;
        }
        regs.b ^= op;
    }
    void bxc(uint8_t unused)
    {
        regs.b ^= regs.c;
    }
    void out(uint8_t combo)
    {
        output.push_back(decode(combo) & 0b111);
    }
    void bdv(uint8_t combo)
    {
        regs.b = regs.a >> decode(combo);
    }
    void cdv(uint8_t combo)
    {
        regs.c = regs.a >> decode(combo);
    }

    bool step()
    {
        if (ip >= instructions.size())
        {
            return false; // HALT
        }
        uint8_t instructon = instructions[ip++];
        uint8_t operand = instructions[ip++];
        i_map[instructon](operand);
        return true;
    }
};

class task
{
private:
    std::filesystem::path file;

public:
    reg take_reg(std::ifstream &in) {
        std::smatch match;
        std::string s;
        std::getline(in, s);
        std::regex_match(s, match, register_line);
        return std::stoull(match[1]);
    }
    std::string take_program(std::ifstream &in) {
        std::smatch match;
        std::string s;
        std::getline(in, s);
        std::regex_match(s, match, program_line);
        return match[1];
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
        std::smatch match;
        reg a, b, c;
        a = take_reg(infile);
        b = take_reg(infile);
        c = take_reg(infile);
        std::getline(infile, s);
        std::string program = take_program(infile);
        computer cmp{a, b, c,program};
        while (cmp.step());
        cmp.print();
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day17" << std::endl;
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
