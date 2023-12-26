#include <gtest/gtest.h>

#include "change_log_checker.hpp"

using namespace change_log_checker;

TEST(TestParingContext, SortVersionDetail)
{
    ParsingContext ctx(ParsingContextConfiguration{"####", "-", {"fix", "feat"}});
    ctx.add_line("#### 1.3.0");
    ctx.add_line("- feat(iie): toto");
    ctx.add_line("- fix(ff_): tata");
    ctx.add_line("- no prefix");
    ctx.add_line("- feat(x_x): dota");
    auto result = ctx.serialize();
    EXPECT_EQ(result, "#### 1.3.0\n- fix(ff_): tata\n- feat(iie): toto\n- feat(x_x): dota\n- no prefix\n\n");
}

TEST(TestParingContext, SortVersionTag)
{
    ParsingContext ctx(ParsingContextConfiguration{"####", "-", {"fix", "feat"}});
    ctx.add_line("#### 1.3.0");
    ctx.add_line("- feat(iie): toto");
    ctx.add_line("#### 1.13.0");
    ctx.add_line("- feat(iie): toto");
    ctx.add_line("");
    ctx.add_line("");
    ctx.add_line("#### 2.3.0");
    ctx.add_line("- feat(iie): toto");
    auto result = ctx.serialize();
    EXPECT_EQ(result,
              "#### 2.3.0\n- feat(iie): toto\n\n#### 1.13.0\n- feat(iie): toto\n\n#### 1.3.0\n- feat(iie): toto\n\n");
}

TEST(TestParingContext, IgnorePrefixedDetail)
{
    ParsingContext ctx(ParsingContextConfiguration{"####", "-", {"fix", "feat"}});
    ctx.add_line("#### 1.3.0");
    ctx.add_line("- feat(iie): toto");
    ctx.add_line("nop");
    auto result = ctx.serialize();
    EXPECT_EQ(result, "#### 1.3.0\n- feat(iie): toto\n\n");
}
