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
    computer(reg a, reg b, reg c, const std::string &s)
    {
        regs.a = a;
        regs.b = b;
        regs.c = c;
        for (auto ch : s)
        {
            if (ch == ',')
            {
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
        fmt::println("{}", std::string(30, '-'));
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
    reg take_reg(std::ifstream &in)
    {
        std::smatch match;
        std::string s;
        std::getline(in, s);
        std::regex_match(s, match, register_line);
        return std::stoull(match[1]);
    }
    std::string take_program(std::ifstream &in)
    {
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
    reg find_next_a(size_t idx, reg a, const std::vector<uint8_t> &expected, const std::string &program)
    {
        if (idx == 0)
        {
            // found it
            return a;
        }
        else
        {
            idx--;
            auto what = expected[idx];
            for (reg i = 0; i < 8; i++)
            {
                reg curr_a = a << 3 | (i & 0b111);
                computer solve{curr_a, 0, 0, program};
                while (solve.step())
                    ;
                if (solve.output.front() == what) // if found what we need
                {
                    auto found = find_next_a(idx, curr_a, expected, program); // going deep
                    if (found != 0)
                    {
                        return found;
                    }
                }
            }
        }
        return 0;
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
        computer cmp{a, b, c, program};
        while (cmp.step())
            ;
        cmp.print();

        // hardcoding solver based on the program in the input.
        // Program: 2,4,1,7,7,5,0,3,4,4,1,7,5,5,3,0
        // assembly-like
        // --------------------------------------
        // bst a ; 2,4 -- b = a & 0b111
        // bxl 7 ; 1,7 -- b^= 0b111
        // cdv b ; 7,5 -- c = a >> b
        // adv 3 ; 0,3 -- a >>= 3
        // bxc   ; 4,4 -- b ^= c
        // bxl 7 ; 1,7 -- b ^= 0b111
        // out b ; 5,5 -- print b & 0b111
        // jnz 0 ; 3,0 -- jnz start:
        // --------------------------------------
        // attempt 3
        // look for A value from the back
        a = find_next_a(cmp.instructions.size(), 0, cmp.instructions, program);
        fmt::println("Starting A: {}", a);
        computer solve{a, 0, 0, program};
        while (solve.step())
            ;
        solve.print();
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
