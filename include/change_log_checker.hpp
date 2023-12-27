#ifndef _CHANGE_LOG_CHECKER_HPP
#include <memory>
#include <regex>
#include <vector>

using namespace std;

namespace change_log_checker
{
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

    [[nodiscard]] static auto _get_tag_reg(const ParsingContextConfiguration &config) noexcept -> regex;
    [[nodiscard]] static auto _get_item_reg(const ParsingContextConfiguration &config) noexcept -> regex;

  public:
    ParsingContext(const ParsingContextConfiguration &config) noexcept;

    void add_line(const string_view &line) noexcept;

    [[nodiscard]] auto serialize() const noexcept -> string;
};

auto check(iostream &stream) noexcept -> string;
}; // namespace change_log_checker

#endif