///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/filesystem/file_manip.hpp"
#include "polos/filesystem/resource.hpp"

#include <gtest/gtest.h>

namespace
{

TEST(FileManipTestFixture, OpenAndRead_When_CorrectFileName)
{
    auto file = polos::fs::ReadFile("test_file.test", std::ios::ate);

    EXPECT_TRUE(file);

    polos::fs::resource& test_file = file.value();

    EXPECT_STREQ(test_file.name.c_str(), "test_file");
    EXPECT_EQ(test_file.size, 513);
    EXPECT_EQ(test_file.uncompressed_size, 513);
    EXPECT_EQ(test_file.data.size(), 513);

    char first_letter = static_cast<char>(test_file.data[0]);
    EXPECT_EQ(first_letter, 'L');
}

TEST(FileManipTestFixture, OpenAndRead_When_IncorrectFileName)
{
    auto file = polos::fs::ReadFile("test_fle.test", std::ios::ate);

    EXPECT_FALSE(file);
}

}// namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
