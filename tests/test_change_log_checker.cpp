#include <gtest/gtest.h>

#include "change_log_checker.hpp"

using namespace change_log_checker;

void run_check(istream &input_stream, ostream &output_stream,
               const ChangeLogCheckerConfiguration &config = change_log_checker::ChangeLogCheckerConfiguration{
                   "####", "-", {"fix", "feat", "chore"}}) noexcept
{
    check(input_stream, output_stream, config);
};

TEST(TestChangeLogChecker, SortVersionDetail)
{
    stringstream input;
    input << "#### 1.3.0\n"
          << "- feat(iie): toto\n"
          << "- fix(ff_): tata\n"
          << "- no prefix\n"
          << "- feat(x_x): dota\n";
    stringstream result;
    run_check(input, result);
    EXPECT_EQ(result.str(), "#### 1.3.0\n- fix(ff_): tata\n- feat(iie): toto\n- feat(x_x): dota\n- no prefix\n\n");
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
    stringstream result;
    run_check(input, result);
    EXPECT_EQ(result.str(),
              "#### 2.3.0\n- feat(iie): toto\n\n#### 1.13.0\n- feat(iie): toto\n\n#### 1.3.0\n- feat(iie): toto\n\n");
}

TEST(TestChangeLogChecker, IgnorePrefixedDetail)
{
    stringstream input;
    input << "#### 1.3.0\n"
          << "- feat(iie): toto\n"
          << "nop\n";
    stringstream result;
    run_check(input, result);
    EXPECT_EQ(result.str(), "#### 1.3.0\n- feat(iie): toto\n\n");
}
