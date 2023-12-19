#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <vector>

using namespace std;

struct VersionDetail
{
    string tag{""};
    vector<string> details;
};

[[nodiscard]] auto parse_version(const string &data, const string &delimiter) noexcept -> vector<int>
{
    vector<int> result;
    size_t prepos{0};
    auto i = data.find(delimiter);
    while (i != -1)
    {
        const auto &part = data.substr(prepos, i);
        result.push_back(stoi(part));
        prepos = i + delimiter.size();
        i = data.find(delimiter, prepos);
    }
    return result;
};

struct ParsingContext
{
    string _tag_prefix;
    string _item_prefix;
    vector<unique_ptr<VersionDetail>> vd;

    void add_line(const string &line) noexcept
    {
        if (line.empty())
        {
            return;
        }

        const regex tag_reg{_tag_prefix.empty() ? "(\\d+\\.\\d+\\.\\d+)" : _tag_prefix + "\\s*(\\d+\\.\\d+\\.\\d+)"};

        std::smatch match;
        if (regex_match(line, match, tag_reg))
        {
            vd.push_back(make_unique<VersionDetail>());
            vd.back()->tag = match[1];
            return;
        }

        std::smatch item_match;
        const regex item_reg{_item_prefix.empty() ? "\\s*(.+)\\s*$" : _item_prefix + "\\s*(.+)\\s*$"};
        if (!regex_match(line, item_match, item_reg))
        {
            return;
        }
        if (vd.size() == 0)
        {
            return;
        }
        vd.back()->details.push_back(item_match[1]);
        return;
    }

    void sort_details() noexcept
    {
        sort(vd.begin(), vd.end(), [](const unique_ptr<VersionDetail> &vd1, const unique_ptr<VersionDetail> &vd2) {
            return parse_version(vd1->tag, ".") > parse_version(vd2->tag, ".");
        });
    }

    [[nodiscard]] auto serialize() const noexcept -> string
    {
        string result;
        for (const auto &i : vd)
        {
            string &&tag{_tag_prefix.empty() ? i->tag + "\n" : _tag_prefix + " " + i->tag + "\n"};
            result.append(tag);
            sort(i->details.begin(), i->details.end(), [](const string &s1, const string &s2) {
                if ((s1.starts_with("- fix") && !s1.starts_with("- fix")) ||
                    (s1.starts_with("- feat") && !s1.starts_with("- feat")) ||
                    (s1.starts_with("- chore") && !s1.starts_with("- chore")))
                {
                    return true;
                }

                return s1 < s2;
            });
            for (const auto &j : i->details)
            {
                result.append(_item_prefix.empty() ? j + "\n" : _item_prefix + " " + j + "\n");
            }
            result.append("\n");
        }
        return result;
    }
};

[[nodiscard]] auto trimmed(string &s) noexcept -> string
{
    auto is_not_space = [](unsigned char ch) { return !isspace(ch); };
    s.erase(s.begin(), find_if(s.begin(), s.end(), is_not_space));
    s.erase(find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
    return s;
}

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
    ParsingContext ctx{"####", "-"};
    while (getline(file, line))
    {
        line = trimmed(line);
        ctx.add_line(line);
    }
    file.close();
    ctx.sort_details();
    std::cout << ctx.serialize();
    return 0;
}