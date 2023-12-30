
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
    // auto file = fstream(file_path);
    // if (!file.is_open())
    // {
    //     std::cout << "Can not open file: " << file_path << "\n";
    //     return 1;
    // }

    ChangeLogCheckerConfiguration config{"####", "-", {"fix", "feat", "chore"}};
    DataFileReader reader(file_path);
    auto result = check(reader.stream(), config);

    if (opt.inplace_write_file)
    {
        ResultFilePrinter(file_path).print(result);
    }
    else
    {
        ResultStreamPrinter(std::cout).print(result);
    }

    return 0;
};