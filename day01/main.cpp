#include <iostream>
#include <filesystem>
#include "../utils.h"

class task
{
private:
    std::filesystem::path file;
public:
    task(const std::filesystem::path &input): file(input)
    {
        std::cout << "Task input: " << file << std::endl;
    }
    void run()
    {
        std::cout << "Starting..." << std::endl;
    }
};

int main(int argc, char *argv[])
{
    std::cout << "Day01" << std::endl;
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
