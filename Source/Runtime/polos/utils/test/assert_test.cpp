#include "polos/utils/assert.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

TEST(AssertTest, EvaluatesSuccessfulConditionExactlyOnce)
{
    int evaluations = 0;
    PL_ASSERT(++evaluations == 1, "Condition must run exactly once.");
    EXPECT_EQ(evaluations, 1);
}

TEST(AssertTest, DoesNotEvaluateFormatArgumentsOnSuccess)
{
    int reasonEvaluations = 0;
    PL_ASSERT(true, "Unnecessary failure reason: {}", ++reasonEvaluations);
    EXPECT_EQ(reasonEvaluations, 0);
}

TEST(AssertTest, CanBeUsedAsOneStatement)
{
    bool reachedElse = false;
    if (true)
        PL_ASSERT(true, "Successful branch.");
    else
        reachedElse = true;
    EXPECT_FALSE(reachedElse);
}

TEST(AssertTest, FailingConditionAborts)
{
#ifdef NDEBUG
    char const* diagnostic = "deliberate assertion failure";
#else
    char const* diagnostic = "false";
#endif
    EXPECT_DEATH(PL_ASSERT(false, "deliberate assertion failure"), diagnostic);
}

#ifdef NDEBUG
TEST(AssertTest, ReleasePrintsRuntimeReasonAndSourceLocation)
{
    std::string const reason = "runtime startup failure";
    EXPECT_DEATH(PL_ASSERT(false, "{}", reason), "Reason: runtime startup failure\nAt .*assert_test.cpp:");
}

TEST(AssertTest, ReleaseRespectsReasonViewLength)
{
    std::string const      buffer = "fatal ignored suffix";
    std::string_view const reason(buffer.data(), 5);
    EXPECT_DEATH(PL_ASSERT(false, "{}", reason), "Reason: fatal\nAt ");
}
#endif

#ifdef NDEBUG
TEST(AssertTest, ReleaseFormatsMultipleArgumentsAndFormatSpecifiers)
{
    std::string const resource = "texture.tux";
    EXPECT_DEATH(
        PL_ASSERT(false, "Resource {} failed with code 0x{:04x}", resource, 42),
        "Reason: Resource texture.tux failed with code 0x002a");
}

TEST(AssertTest, ReleaseEvaluatesFailureArgumentsExactlyOnce)
{
    int evaluations = 0;
    EXPECT_DEATH(PL_ASSERT(false, "argument evaluations: {}", ++evaluations), "Reason: argument evaluations: 1");
}
#endif
