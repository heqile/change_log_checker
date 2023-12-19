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

struct ParsingContext
{
    string _tag_prefix;
    string _item_prefix;
    vector<string> _order;
    vector<shared_ptr<VersionDetail>> vertion_detail{};
    shared_ptr<VersionDetail> _current_vertion_detail{nullptr};

    void add_line(const string &line) noexcept
    {
        if (line.empty())
        {
            return;
        }

        const regex tag_reg{_tag_prefix.empty() ? "(\\d+)\\.(\\d+)\\.(\\d+)"
                                                : _tag_prefix + "\\s*(\\d+)\\.(\\d+)\\.(\\d+)"};

        std::smatch match;
        if (regex_match(line, match, tag_reg))
        {
            // append new detail object with version tag
            _current_vertion_detail = make_shared<VersionDetail>();
            _current_vertion_detail->tag = tuple<int, int, int>{stoi(match[1]), stoi(match[2]), stoi(match[3])};
            vertion_detail.push_back(_current_vertion_detail);
            // sort versions
            sort(vertion_detail.begin(), vertion_detail.end(),
                 [](const shared_ptr<VersionDetail> &vd1, const shared_ptr<VersionDetail> &vd2) {
                     return vd1->tag > vd2->tag;
                 });
            return;
        }

        if (!_current_vertion_detail)
        {
            return;
        }
        std::smatch item_match;
        const regex item_reg{_item_prefix.empty() ? "\\s*(.+)\\s*$" : _item_prefix + "\\s*(.+)\\s*$"};
        if (!regex_match(line, item_match, item_reg))
        {
            return;
        }
        // append detail
        const string &detail = item_match[1];
        // sort details
        using order_vec_t = decltype(_order);
        order_vec_t::size_type weight = _order.size();
        for (order_vec_t::size_type i = 0; i < _order.size(); i++)
        {
            if (detail.starts_with(_order[i]))
            {
                weight = i;
                break;
            }
        }
        _current_vertion_detail->details.push_back(pair{weight, detail});
        sort(begin(_current_vertion_detail->details), end(_current_vertion_detail->details));
        return;
    }

    [[nodiscard]] auto serialize() const noexcept -> string
    {
        string result;
        for (const auto &i : vertion_detail)
        {
            const auto [major, minor, patch] = i->tag;
            auto &&tag = to_string(major) + "." + to_string(minor) + "." + to_string(patch);
            result += _tag_prefix.empty() ? tag + "\n" : _tag_prefix + " " + tag + "\n";
            for (const auto &j : i->details)
            {
                result += _item_prefix.empty() ? j.second + "\n" : _item_prefix + " " + j.second + "\n";
            }
            result += "\n";
        }
        return result;
    }
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

    string line;
    ParsingContext ctx{"####", "-", {"fix", "feat", "chore"}};
    while (getline(file, line))
    {
        ctx.add_line(line);
    }
    file.close();
    std::cout << ctx.serialize();
    return 0;
}