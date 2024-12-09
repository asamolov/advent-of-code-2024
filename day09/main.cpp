#include <iostream>
#include <fstream>
#include <filesystem>
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
    void run()
    {
        execution_timer timer;
        std::cout << "Starting..." << std::endl;
        std::ifstream infile(file);
        std::string s;
        int n = 0;
        std::vector<size_t> output;
        std::getline(infile, s);
        std::vector<unsigned int> disk_map;
        for (size_t i = 0; i < s.size(); i++)
        {
            disk_map.push_back(s.at(i) - '0');
        }

        // dumb approach
        for (size_t i = 0; i < disk_map.size(); i++)
        {
            size_t file;
            if (i % 2 == 0)
            {
                file = i / 2;
            }
            else
            {
                file = std::numeric_limits<size_t>::max();
            }
            auto n = disk_map[i];
            while (n > 0)
            {
                output.push_back(file);
                n--;
            }
        }

        // moving stuff around
        size_t back = output.size() - 1;
        while (output[back] == std::numeric_limits<size_t>::max())
        {
            back--;
        }

        for (size_t front = 0; front < back; front++)
        {
            if (output[front] == std::numeric_limits<size_t>::max())
            {
                output[front] = output[back];
                output[back] = std::numeric_limits<size_t>::max();
                while (output[back] == std::numeric_limits<size_t>::max()) // pull back
                {
                    back--;
                }
            }
        }

        unsigned long long alt_checksum = 0;
        // checksum
        for (size_t i = 0; i < output.size() - 1 && output[i] != std::numeric_limits<size_t>::max(); i++)
        {
            alt_checksum += i * output[i];
        }

        // iterating both back and forward
        unsigned long long checksum = 0;
        unsigned long long block = 0;
        size_t back_file = (disk_map.size() - 1) / 2 + 1;
        unsigned int remainder = 0;
        for (size_t i = 0; i < back_file * 2 - 1; i++)
        {
            // if even - calculate checksum (0, 2, ...)
            if (i % 2 == 0)
            {
                unsigned int len = disk_map[i];
                checksum += chksum(i / 2 /*file*/, len, block);
                block += len;
            }
            else
            {
                // fillin empty space from the back
                unsigned int free_space = disk_map[i];
                while (free_space > 0)
                {
                    // take from back
                    if (remainder == 0)
                    {
                        back_file--; // next file
                        remainder = disk_map[back_file * 2];
                    }
                    size_t take = std::min(remainder, free_space);
                    checksum += chksum(back_file, take, block);
                    remainder -= take;
                    free_space -= take;
                    block += take;
                }
            }
        }
        // remainder
        checksum += chksum(back_file, remainder, block);

        fmt::println("checksum {}, alt {}", checksum, alt_checksum);
    }
    unsigned long long chksum(size_t file, size_t take, unsigned long long block)
    {
        return (file /*file*/) * take * (block + block + take - 1) / 2;
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day09" << std::endl;
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
