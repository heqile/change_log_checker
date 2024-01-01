#ifndef _CHANGE_LOG_CHECKER_HPP
#include <fstream>
#include <memory>
#include <regex>
#include <vector>

using namespace std;

namespace change_log_checker
{
struct Options
{
    bool inplace_write_file{false};
    string input_file_path;
    string config_file_path;

    Options(const vector<string_view> &data) noexcept;
    // input_file -i -c config.txt
};

class DataReader
{
  public:
    virtual unique_ptr<istream> get_istream() const noexcept = 0;
};

class DataStringReader : public DataReader
{
  private:
    const string _data;

  public:
    DataStringReader(const string &data) noexcept;

    virtual unique_ptr<istream> get_istream() const noexcept;
};

class DataFileReader : public DataReader
{
  private:
    const string _input_file_path;

  public:
    DataFileReader(const string &file_name) noexcept;

    virtual unique_ptr<istream> get_istream() const noexcept;
};

class ResultPrinter
{
  public:
    virtual void print(const string &data) const noexcept = 0;
};

class ResultFilePrinter : public ResultPrinter
{
  private:
    string _output_file_path;

  public:
    ResultFilePrinter(const string &file_path) noexcept;
    virtual void print(const string &data) const noexcept;
};

class ResultStreamPrinter : public ResultPrinter
{
  private:
    ostream &_output_stream;

  public:
    ResultStreamPrinter(ostream &output_stream) noexcept;
    virtual void print(const string &data) const noexcept;
};

struct VersionDetail
{
    tuple<int, int, int> tag;
    vector<pair<int, string>> details;
};

struct ChangeLogCheckerConfiguration
{
    const string tag_prefix;
    const string item_prefix;
    const vector<string> order;
};

class ParsingContext
{
  private:
    const ChangeLogCheckerConfiguration _config;
    const regex _tag_reg;
    const regex _item_reg;
    vector<shared_ptr<VersionDetail>> _vertion_detail{};
    shared_ptr<VersionDetail> _current_vertion_detail{nullptr};

    [[nodiscard]] static auto _get_tag_reg(const ChangeLogCheckerConfiguration &config) noexcept -> regex;
    [[nodiscard]] static auto _get_item_reg(const ChangeLogCheckerConfiguration &config) noexcept -> regex;

  public:
    ParsingContext(const ChangeLogCheckerConfiguration &config) noexcept;

    void add_line(const string_view &line) noexcept;

    [[nodiscard]] auto serialize() const noexcept -> string;
};

void check(const DataReader &data_reader, unique_ptr<ResultPrinter> result_printer,
           const ChangeLogCheckerConfiguration &config) noexcept;
}; // namespace change_log_checker

#endif