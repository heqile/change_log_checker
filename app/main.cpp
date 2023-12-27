
#include <fstream>
#include <iostream>

#include "change_log_checker.hpp"

using namespace std;

auto main(int argc, char *argv[]) -> int
{
    if (argc != 2)
    {
        std::cout << "Usage: change_log_checker [file path]\n";
        return 0;
    }

    auto *file_path = argv[1];
    auto file = fstream(file_path);
    if (!file.is_open())
    {
        std::cout << "Can not open file: " << file_path << "\n";
        return 1;
    }
    std::cout << change_log_checker::check(file);

    file.close();

    return 0;
};