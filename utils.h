#pragma once
#include <iostream>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <source_location>

void usage(char *path)
{
    std::cout << "Usage: " << path << " <input.txt>" << std::endl;
    std::cout << "where <input.txt> - file with the command input" << std::endl;
}

bool ensure_input(int argc, char *argv[], std::filesystem::path &input)
{
    if (argc == 2)
    {
        input.assign(argv[1]);
        if (std::filesystem::exists(input))
        {
            return true;
        }
    }
    return false;
}

namespace aoc
{
    template <typename T = int>
    struct point
    {
        T row;
        T col;
        point() : row(), col() {}
        point(const T &row, const T &col) : row(row), col(col) {}
        constexpr bool operator==(const point &other) const
        {
            return row == other.row && col == other.col;
        }
        friend bool operator<(const point &lhs, const point &rhs)
        {
            if (lhs.row == rhs.row)
            {
                return lhs.col < rhs.col;
            }
            return lhs.row < rhs.row;
        }
        friend constexpr std::strong_ordering operator<=>(const point &lhs, const point &rhs)
        {
            if (lhs.row < rhs.row or (lhs.row == rhs.row and lhs.col < rhs.col))
                return std::strong_ordering::less;
            if (lhs.row > rhs.row or (lhs.row == rhs.row and lhs.col > rhs.col))
                return std::strong_ordering::greater;
            return std::strong_ordering::equivalent;
        }
    };
    struct dir
    {
        int d_row;
        int d_col;
        dir() : d_row(0), d_col(0) {}
        dir(int d_row, int d_col) : d_row(d_row), d_col(d_col) {}
        template <typename T = int>
        point<T> move(const point<T> &pt) const
        {
            return point{pt.row + d_row, pt.col + d_col};
        }
        dir rot90cw()
        {
            // up: -1, 0; down 1, 0; left 0, -1; right 0, 1
            // 90 cw rotation is achieved by multiplying direction vector by rotation matrix
            // [ 0  1 ] [ x ]
            // [ -1 0 ] [ y ]
            return dir{d_col, -d_row};
        }
        dir rot90ccw()
        {
            // up: -1, 0; down 1, 0; left 0, -1; right 0, 1
            // 90 ccw rotation is achieved by multiplying direction vector by rotation matrix
            // [ 0  -1 ] [ x ]
            // [ 1 0 ] [ y ]
            return dir{-d_col, d_row};
        }
        dir reverse() const
        {
            return dir{-d_row, -d_col};
        }
        constexpr bool operator==(const dir &other) const
        {
            return d_row == other.d_row && d_col == other.d_col;
        }
        static dir from_ch(const char &ch);
        operator char() const;

        friend constexpr std::strong_ordering operator<=>(const dir &lhs, const dir &rhs)
        {
            if (lhs.d_row < rhs.d_row or (lhs.d_row == rhs.d_row and lhs.d_col < rhs.d_col))
                return std::strong_ordering::less;
            if (lhs.d_row > rhs.d_row or (lhs.d_row == rhs.d_row and lhs.d_col > rhs.d_col))
                return std::strong_ordering::greater;
            return std::strong_ordering::equivalent;
        }
    };
    const dir UP{-1, 0};
    const dir DOWN{1, 0};
    const dir LEFT{0, -1};
    const dir RIGHT{0, 1};
    dir aoc::dir::from_ch(const char &ch)
    {
        switch (ch)
        {
        case '^':
            return UP;
        case 'v':
            return DOWN;
        case '<':
            return LEFT;
        case '>':
            return RIGHT;
        }
        return dir(0, 0);
    }
    inline dir::operator char() const
    {
        if (d_row == -1 && d_col ==  0) return '^';
        if (d_row ==  1 && d_col ==  0) return 'v';
        if (d_row ==  0 && d_col ==  1) return '>';
        if (d_row ==  0 && d_col == -1) return '<';
        return ' ';
    }
};

template <class Resolution = std::chrono::milliseconds>
class execution_timer
{
public:
    using clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                                     std::chrono::high_resolution_clock,
                                     std::chrono::steady_clock>;

private:
    const clock::time_point start = clock::now();
    const std::string fn;

public:
    execution_timer(const std::source_location &location = std::source_location::current()) : fn(location.function_name())
    {
    }
    ~execution_timer()
    {
        const auto end = clock::now();
        fmt::println("total {} elapsed: {}", fn, std::chrono::duration_cast<Resolution>(end - start));
    }

    inline void stop(const std::string &tag = "!")
    {
        const auto end = clock::now();
        fmt::println("{} elapsed: {}", tag, std::chrono::duration_cast<Resolution>(end - start));
    }

}; // ExecutionTimer