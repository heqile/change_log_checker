#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <vector>

using namespace std;

struct VersionDetail
{
    string tag;
    vector<string> details;
};

auto parse_version(const string &data, const string &delimiter) noexcept -> vector<int>
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
    vector<unique_ptr<VersionDetail>> vd;

    void add_line(const string &line) noexcept
    {
        if (line.empty())
        {
            return;
        }

        const regex reg{R"(\d+\.\d+\.\d+)"};
        if (regex_match(line, reg))
        {
            vd.push_back(make_unique<VersionDetail>());
            vd.back()->tag = line;
            return;
        }
        if (!line.starts_with("-"))
        {
            return;
        }
        if (vd.size() == 0)
        {
            return;
        }
        auto &current = vd.back();
        current->details.push_back(line);
        return;
    }

    void sort_details() noexcept
    {
        sort(vd.begin(), vd.end(), [](const unique_ptr<VersionDetail> &vd1, const unique_ptr<VersionDetail> &vd2) {
            return parse_version(vd1->tag, ".") > parse_version(vd2->tag, ".");
        });
    }

    auto serialize() const noexcept -> string
    {
        string result;
        for (const auto &i : vd)
        {
            result.append(i->tag);
            result.append("\n");
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
                result.append(j);
                result.append("\n");
            }
            result.append("\n");
        }
        return result;
    }
};

auto trimmed(string &s) noexcept -> string
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
    ParsingContext ctx;
    while (getline(file, line))
    {
        line = trimmed(line);
        ctx.add_line(line);
    }
    file.close();
    ctx.sort_details();
    std::cout << ctx.serialize() << "\n";
    return 0;
}