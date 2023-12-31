#include <gtest/gtest.h>

#include "change_log_checker.hpp"

using namespace change_log_checker;

void run_check(string_view data, const shared_ptr<ostream> &output_stream) noexcept
{
    DataStringReader reader(data);
    check(reader, make_unique<ResultStreamPrinter>(output_stream),
          change_log_checker::ChangeLogCheckerConfiguration{"####", "-", {"fix", "feat", "chore"}});
};

TEST(TestChangeLogChecker, SortVersionDetail)
{
    stringstream input;
    input << "#### 1.3.0\n"
          << "- feat(iie): toto\n"
          << "- fix(ff_): tata\n"
          << "- no prefix\n"
          << "- feat(x_x): dota\n";
    shared_ptr<ostringstream> result = make_shared<ostringstream>();
    run_check(input.str(), result);
    EXPECT_EQ(result->str(), "#### 1.3.0\n- fix(ff_): tata\n- feat(iie): toto\n- feat(x_x): dota\n- no prefix\n\n");
}

TEST(TestChangeLogChecker, SortVersionTag)
{
    stringstream input;
    input << "#### 1.3.0\n"
          << "- feat(iie): toto\n"
          << "#### 1.13.0\n"
          << "- feat(iie): toto\n"
          << "\n"
          << "\n"
          << "#### 2.3.0\n"
          << "- feat(iie): toto\n";
    shared_ptr<ostringstream> result = make_shared<ostringstream>();
    run_check(input.str(), result);
    EXPECT_EQ(result->str(),
              "#### 2.3.0\n- feat(iie): toto\n\n#### 1.13.0\n- feat(iie): toto\n\n#### 1.3.0\n- feat(iie): toto\n\n");
}

TEST(TestChangeLogChecker, IgnorePrefixedDetail)
{
    stringstream input;
    input << "#### 1.3.0\n"
          << "- feat(iie): toto\n"
          << "nop\n";
    shared_ptr<ostringstream> result = make_shared<ostringstream>();
    run_check(input.str(), result);
    EXPECT_EQ(result->str(), "#### 1.3.0\n- feat(iie): toto\n\n");
}
