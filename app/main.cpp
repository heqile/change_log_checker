
#include <fstream>
#include <iostream>

#include "change_log_checker.hpp"

using namespace std;
using namespace change_log_checker;

auto main(int argc, char *argv[]) -> int
{
    Options opt(vector<string_view>(argv + 1, argv + argc));
    if (opt.input_file_path.empty())
    {
        std::cout << "Usage: change_log_checker [input_file_path]\n";
        return 0;
    }

    const auto &file_path = opt.input_file_path;
    auto file = fstream(file_path);
    if (!file.is_open())
    {
        std::cout << "Can not open file: " << file_path << "\n";
        return 1;
    }

    ChangeLogCheckerConfiguration config{"####", "-", {"fix", "feat", "chore"}};

    if (opt.inplace_write_file)
    {
        check(file, ResultFilePrinter(file), config);
    }
    else
    {
        check(file, ResultStdOutPrinter(std::cout), config);
    }

    file.close();

    return 0;
};