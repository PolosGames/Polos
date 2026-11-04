///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/filesystem/file.hpp"
#include "polos/filesystem/file_manip.hpp"

#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace
{
TEST(FileManipTestFixture, OpenAndReadWhenCorrectFileName)
{
    auto file = polos::fs::ReadFile("test_file.test"_path);

    ASSERT_TRUE(file);

    polos::fs::File const& testFile = file.value();

    EXPECT_STREQ(testFile.stemName.c_str(), "test_file");
    EXPECT_EQ(testFile.size, 513U);
    EXPECT_EQ(testFile.uncompressedSize, 513U);
    ASSERT_EQ(testFile.data.size(), 513U);

    char const firstLetter = static_cast<char>(testFile.data[0]);

    EXPECT_EQ(firstLetter, 'L');
}

TEST(FileManipTestFixture, OpenAndReadWhenIncorrectFileName)
{
    auto file = polos::fs::ReadFile("test_fle.test"_path);

    EXPECT_FALSE(file);
}

class BinaryFileTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        directory = std::filesystem::temp_directory_path() /
                    ("polos-files-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directory(directory);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(directory);
    }

    std::filesystem::path directory;
};

TEST_F(BinaryFileTest, PreservesBinaryBytesAndCustomMetadata)
{
    std::array<unsigned char, 6U> const expected{0x00, 0x0A, 0x0D, 0x1A, 0x80, 0xFF};
    auto const                          path = directory / "payload.bin";
    {
        std::ofstream output{path, std::ios::binary};
        output.write(reinterpret_cast<char const*>(expected.data()), expected.size());
        ASSERT_TRUE(output);
    }

    auto const result = polos::fs::ReadFile("custom payload", path);

    ASSERT_TRUE(result);
    EXPECT_EQ(result->customName, "custom payload");
    EXPECT_EQ(result->stemName, "payload");
    EXPECT_EQ(result->path, path);
    EXPECT_EQ(result->size, expected.size());
    EXPECT_EQ(result->uncompressedSize, expected.size());
    ASSERT_EQ(result->data.size(), expected.size());

    for (std::size_t i{0U}; i < expected.size(); ++i)
    {
        SCOPED_TRACE(i);
        EXPECT_EQ(std::to_integer<unsigned char>(result->data[i]), expected[i]);
    }
}

TEST_F(BinaryFileTest, EmptyFileIsASuccessfulEmptyBuffer)
{
    auto const path = directory / "empty.bin";
    {
        std::ofstream output{path, std::ios::binary};
        ASSERT_TRUE(output);
    }

    auto const result = polos::fs::ReadFile(path);

    ASSERT_TRUE(result);
    EXPECT_EQ(result->customName, "empty.bin");
    EXPECT_EQ(result->size, 0U);
    EXPECT_EQ(result->uncompressedSize, 0U);
    EXPECT_TRUE(result->data.empty());
}

}// namespace
