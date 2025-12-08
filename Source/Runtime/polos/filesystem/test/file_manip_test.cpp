///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/filesystem/file_manip.hpp"
#include "polos/filesystem/resource.hpp"

#include <gtest/gtest.h>

namespace
{

TEST(FileManipTestFixture, OpenAndReadWhenCorrectFileName)
{
    auto file = polos::fs::ReadFile("test_file.test"_path);

    EXPECT_TRUE(file);

    polos::fs::resource& test_file = file.value();

    EXPECT_STREQ(test_file.stem_name.c_str(), "test_file");
    EXPECT_EQ(test_file.size, 513);
    EXPECT_EQ(test_file.uncompressed_size, 513);
    EXPECT_EQ(test_file.data.size(), 513);

    char const first_letter = static_cast<char>(test_file.data[0]);
    EXPECT_EQ(first_letter, 'L');
}

TEST(FileManipTestFixture, OpenAndReadWhenIncorrectFileName)
{
    auto file = polos::fs::ReadFile("test_fle.test"_path);

    EXPECT_FALSE(file);
}

}// namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
