#include <iostream>
#include <filesystem>

int main(int argc, char *argv[])
{
    std::cout << "template" << std::endl;
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
    std::cout << "args:" << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << '\t' << argv[i] << std::endl;
    }
    return 0;
}
