#pragma once
#include <iostream>
#include <filesystem>

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
