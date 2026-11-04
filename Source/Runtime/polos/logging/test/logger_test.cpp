#include "polos/logging/logger.hpp"

#include <gtest/gtest.h>
#include <quill/LogMacros.h>

#include <array>
#include <string>

namespace polos::logging
{
namespace
{
TEST(LoggerTest, ContextsRouteToDistinctStableNamedLoggers)
{
    auto& logging = Logger::Instance();

    struct Context
    {
        quill::Logger* logger;
        char const*    name;
    };

    std::array const contexts{
        Context{logging.GetPolosLogger(), "POLOS"},
        Context{logging.GetPollyLogger(), "POLLY"},
        Context{logging.GetAppLogger(), "APP"},
    };

    for (auto const& context : contexts)
    {
        SCOPED_TRACE(context.name);
        ASSERT_NE(context.logger, nullptr);
        EXPECT_EQ(context.logger->get_logger_name(), context.name);
    }

    EXPECT_NE(contexts[0].logger, contexts[1].logger);
    EXPECT_NE(contexts[0].logger, contexts[2].logger);
    EXPECT_NE(contexts[1].logger, contexts[2].logger);
    EXPECT_EQ(Logger::Instance().GetAppLogger(), contexts[2].logger);
}

TEST(LoggerTest, FlushMakesQueuedFormattedMessagesVisible)
{
    auto* logger = Logger::Instance().GetAppLogger();
    ASSERT_NE(logger, nullptr);
    FlushLogger(logger);
    ::testing::internal::CaptureStdout();

    QUILL_LOG_INFO(logger, "flush verification value={}", 731);
    FlushLogger(logger);
    auto const output = ::testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("APP"), std::string::npos);
    EXPECT_NE(output.find("flush verification value=731"), std::string::npos);
}

TEST(LoggerTest, NullFlushReportsErrorWithoutCrashing)
{
    auto* logger = Logger::Instance().GetPolosLogger();
    ASSERT_NE(logger, nullptr);
    FlushLogger(logger);
    ::testing::internal::CaptureStdout();

    FlushLogger(nullptr);
    FlushLogger(logger);
    auto const output = ::testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Cannot flush a null logger!"), std::string::npos);
}

}// namespace
}// namespace polos::logging
