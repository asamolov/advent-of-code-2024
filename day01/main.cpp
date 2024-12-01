#include <iostream>
#include <fstream>
#include <filesystem>
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
        std::cout << "Starting..." << std::endl;
        std::vector<int> left, right;

        std::ifstream infile(file);
        int l, r;
        while (infile >> l >> r)
        {
            left.push_back(l);
            right.push_back(r);
        }
        std::sort(left.begin(), left.end());
        std::sort(right.begin(), right.end());

        auto it_left = left.cbegin();
        auto it_right = right.cbegin();

        int distance = 0;
        while (it_left != left.cend())
        {
            distance += std::abs(*it_left - *it_right);
            it_left++;
            it_right++;
        }
        return std::to_string(distance);
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
        auto result = task.run();
        std::cout << "Result: " << result << std::endl;
    }
    else
    {
        usage(argv[0]);
    }
    return 0;
}
