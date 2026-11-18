#include "polos/logging/log_macros.hpp"

#include <gtest/gtest.h>

#include <string>

namespace polos::logging
{
namespace
{

TEST(LoggerTest, ContextsRouteToDistinctNamedLoggers)
{
    ::testing::internal::CaptureStdout();

    LogInfoCtx(LOG_CTX_POLOS, "Polos context");
    LogInfoCtx(LOG_CTX_POLLY, "Polly context");
    LogInfoCtx(LOG_CTX_APP, "App context");
    FlushLogger(LOG_CTX_POLOS);
    FlushLogger(LOG_CTX_POLLY);
    FlushLogger(LOG_CTX_APP);

    auto const output = ::testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("POLOS"), std::string::npos);
    EXPECT_NE(output.find("POLLY"), std::string::npos);
    EXPECT_NE(output.find("APP"), std::string::npos);
    EXPECT_NE(output.find("Polos context"), std::string::npos);
    EXPECT_NE(output.find("Polly context"), std::string::npos);
    EXPECT_NE(output.find("App context"), std::string::npos);
}

TEST(LoggerTest, FlushMakesFormattedMessagesAndCallerLocationVisible)
{
    ::testing::internal::CaptureStdout();

    LogInfo("flush verification value={}", 731);
    FlushLogger(LOG_CTX_APP);

    auto const output = ::testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("APP"), std::string::npos);
    EXPECT_NE(output.find("logger_test.cpp:"), std::string::npos);
    EXPECT_NE(output.find("flush verification value=731"), std::string::npos);
}

TEST(LoggerTest, DisabledMessagesDoNotEvaluateArguments)
{
    int calls = 0;
    LogTrace("disabled trace {}", ++calls);
    EXPECT_EQ(calls, 0);
}

}// namespace
}// namespace polos::logging
