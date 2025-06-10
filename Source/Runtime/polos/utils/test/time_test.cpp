//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/utils/time.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>
namespace
{

TEST(TimeTestFixture, ElapsedTimeTest)
{
    polos::TimePoint start = polos::utils::GetTimeNow();

    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(1s);

    polos::TimePoint end = polos::utils::GetTimeNow();

    auto elapsed_time = end - start;

    EXPECT_GE(elapsed_time, polos::utils::ConvertToMicroseconds(1s));
}

}// namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
