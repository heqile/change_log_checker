#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <vector>

using namespace std;

struct VersionDetail
{
    tuple<int, int, int> tag;
    vector<pair<int, string>> details;
};

struct ParsingContextConfiguration
{
    const string tag_prefix;
    const string item_prefix;
    const vector<string> order;
};

class ParsingContext
{
  private:
    const ParsingContextConfiguration _config;
    const regex _tag_reg;
    const regex _item_reg;
    vector<shared_ptr<VersionDetail>> _vertion_detail{};
    shared_ptr<VersionDetail> _current_vertion_detail{nullptr};

    static regex _get_tag_reg(const ParsingContextConfiguration &config) noexcept
    {
        return regex(config.tag_prefix.empty() ? "(\\d+)\\.(\\d+)\\.(\\d+)"
                                               : config.tag_prefix + "\\s*(\\d+)\\.(\\d+)\\.(\\d+)");
    };

    static regex _get_item_reg(const ParsingContextConfiguration &config) noexcept
    {
        return regex(config.item_prefix.empty() ? "\\s*(.+)\\s*$" : config.item_prefix + "\\s*(.+)\\s*$");
    };

  public:
    ParsingContext(const ParsingContextConfiguration &config) noexcept
        : _config{config}, _tag_reg{_get_tag_reg(config)}, _item_reg{_get_item_reg(config)} {};

    void add_line(const string_view &line) noexcept
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
            sort(_vertion_detail.begin(), _vertion_detail.end(),
                 [](const shared_ptr<VersionDetail> &vd1, const shared_ptr<VersionDetail> &vd2) {
                     return vd1->tag > vd2->tag;
                 });
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
        using order_vec_t = decltype(_config.order);
        order_vec_t::size_type weight, order_list_size = _config.order.size();
        for (order_vec_t::size_type i = 0; i < order_list_size; i++)
        {
            if (detail.starts_with(_config.order[i]))
            {
                weight = i;
                break;
            }
        }
        _current_vertion_detail->details.push_back(pair{weight, detail});
        sort(begin(_current_vertion_detail->details), end(_current_vertion_detail->details));
        return;
    };

    [[nodiscard]] auto serialize() const noexcept -> string
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
};

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

    ParsingContextConfiguration config{"####", "-", {"fix", "feat", "chore"}};
    ParsingContext ctx(config);
    string line;
    while (getline(file, line))
    {
        ctx.add_line(line);
    }
    file.close();
    std::cout << ctx.serialize();
    return 0;
};