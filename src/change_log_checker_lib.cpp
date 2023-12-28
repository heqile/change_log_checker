#include <algorithm>
#include <memory>
#include <ranges>
#include <regex>
#include <vector>

#include "change_log_checker.hpp"

using namespace std;

namespace change_log_checker
{

Options::Options(const vector<string_view> &data) noexcept
{
    int i = 0;
    while (i < data.size())
    {
        const string_view &opt = data[i];
        if (opt == "-i")
        {
            inplace_write_file = true;
            i += 1;
        }
        else if (opt == "-c")
        {
            i += 1;
            config_file_path = data[i]; // next element
            i += 1;
        }
        else if (input_file_path.empty())
        {
            input_file_path = opt;
            i += 1;
        }
        else
        {
            i += 1;
        }
    }
};

ResultFilePrinter::ResultFilePrinter(ostream &output_stream) noexcept : _output_stream(output_stream){};

void ResultFilePrinter::print(const string &data) const noexcept
{
    _output_stream.clear();
    _output_stream.seekp(0);
    _output_stream << data;
    _output_stream.flush();
};

ResultStdOutPrinter::ResultStdOutPrinter(ostream &output_stream) noexcept : _output_stream(output_stream){};

void ResultStdOutPrinter::print(const string &data) const noexcept
{
    _output_stream << data;
};

auto ParsingContext::_get_tag_reg(const ChangeLogCheckerConfiguration &config) noexcept -> regex
{
    return regex(config.tag_prefix.empty() ? "(\\d+)\\.(\\d+)\\.(\\d+)"
                                           : config.tag_prefix + "\\s*(\\d+)\\.(\\d+)\\.(\\d+)");
};

auto ParsingContext::_get_item_reg(const ChangeLogCheckerConfiguration &config) noexcept -> regex
{
    return regex(config.item_prefix.empty() ? "\\s*(.+)\\s*$" : config.item_prefix + "\\s*(.+)\\s*$");
};

ParsingContext::ParsingContext(const ChangeLogCheckerConfiguration &config) noexcept
    : _config{config}, _tag_reg{_get_tag_reg(config)}, _item_reg{_get_item_reg(config)} {};

void ParsingContext::add_line(const string_view &line) noexcept
{
    if (line.empty())
    {
        return;
    }

    std::match_results<string_view::const_iterator> match;
    if (regex_match(line.cbegin(), line.cend(), match, _tag_reg))
    {
        // append new detail object with version tag
        _current_vertion_detail = make_shared<VersionDetail>();
        _current_vertion_detail->tag = tuple<int, int, int>{stoi(match[1]), stoi(match[2]), stoi(match[3])};
        _vertion_detail.push_back(_current_vertion_detail);
        // sort versions
        ranges::sort(_vertion_detail, std::greater<>{}, [](auto const &i) { return i->tag; });
        return;
    }

    if (!_current_vertion_detail)
    {
        return;
    }

    std::match_results<string_view::const_iterator> item_match;
    if (!regex_match(line.cbegin(), line.cend(), item_match, _item_reg))
    {
        return;
    }
    // append detail
    const string &detail = item_match[1];
    // sort details
    size_t order_list_size = _config.order.size();
    size_t weight{order_list_size};
    for (size_t i = 0; i < order_list_size; i++)
    {
        if (detail.starts_with(_config.order[i]))
        {
            weight = i;
            break;
        }
    }
    _current_vertion_detail->details.push_back(pair{weight, detail});
    ranges::sort(_current_vertion_detail->details);
    return;
};

auto ParsingContext::serialize() const noexcept -> string
{
    string result;
    for (const auto &i : _vertion_detail)
    {
        const auto [major, minor, patch] = i->tag;
        auto &&tag = to_string(major) + "." + to_string(minor) + "." + to_string(patch);
        result += _config.tag_prefix.empty() ? tag + "\n" : _config.tag_prefix + " " + tag + "\n";
        for (const auto &j : i->details)
        {
            result += _config.item_prefix.empty() ? j.second + "\n" : _config.item_prefix + " " + j.second + "\n";
        }
        result += "\n";
    }
    return result;
};

void check(istream &input_stream, const ResultPrinter &printer, const ChangeLogCheckerConfiguration &config) noexcept
{
    change_log_checker::ParsingContext ctx(config);
    string line;
    while (getline(input_stream, line))
    {
        ctx.add_line(line);
    }

    printer.print(ctx.serialize());
};
}; // namespace change_log_checker
