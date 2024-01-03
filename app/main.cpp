
#include <fstream>
#include <iostream>

#include "change_log_checker.hpp"

using namespace std;
using namespace change_log_checker;

auto create_result_printer(const Options &opt) noexcept -> unique_ptr<ResultPrinter>
{
    if (opt.inplace_write_file)
    {
        return make_unique<ResultFilePrinter>(opt.input_file_path);
    }
    else
    {
        return make_unique<ResultStreamPrinter>(shared_ptr<ostream>(&std::cout));
    }
};

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

    check(reader, create_result_printer(opt), config);

    return 0;
};