#include "polos/resource/image.hpp"
#include "polos/resource/image/image_loader.hpp"
#include "polos/resource/storage.hpp"

#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <vector>

namespace
{

class ImageLoaderTest : public ::testing::Test
{
protected:
    std::filesystem::path directory;

    void SetUp() override
    {
        static std::atomic<unsigned> serial{};
        directory = std::filesystem::temp_directory_path() /
                    ("polos-image-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
                     "-" + std::to_string(serial++));
        std::filesystem::create_directory(directory);
    }

    void TearDown() override
    { std::filesystem::remove_all(directory); }
};

TEST_F(ImageLoaderTest, DecodesPixelsFromValidImage)
{
    // One uncompressed, top-left-origin RGBA pixel in TGA's BGRA byte order.
    std::array<unsigned char, 22> const tga{
        0,
        0,
        2,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        32,
        0x28,
        0x33,
        0x22,
        0x11,
        0xFF,
    };
    auto path = directory / "pixel.tga";
    {
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<char const*>(tga.data()), static_cast<std::streamsize>(tga.size()));
    }

    auto image = polos::resource::image::LoadImage(path);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->width, 1);
    EXPECT_EQ(image->height, 1);
    EXPECT_EQ(image->components, 4);
    ASSERT_EQ(image->data.size(), 4U);
    EXPECT_EQ(image->data[0], 0x11);
    EXPECT_EQ(image->data[1], 0x22);
    EXPECT_EQ(image->data[2], 0x33);
    EXPECT_EQ(image->data[3], 0xFF);
}

TEST_F(ImageLoaderTest, ExpandsRgbAndGrayscaleInputsToCompleteRgbaBuffers)
{
    struct Fixture
    {
        char const*                  name;
        unsigned char                imageType;
        unsigned char                bitsPerPixel;
        unsigned char                descriptor;
        std::vector<unsigned char>   source;
        std::array<unsigned char, 8> expected;
    };
    std::array<Fixture, 3> const fixtures{
        Fixture{
            "rgb",
            2,
            24,
            0x20,
            {0x33, 0x22, 0x11, 0x66, 0x55, 0x44},
            {0x11, 0x22, 0x33, 0xFF, 0x44, 0x55, 0x66, 0xFF}
        },
        Fixture{"gray", 3, 8, 0x20, {0x22, 0x66}, {0x22, 0x22, 0x22, 0xFF, 0x66, 0x66, 0x66, 0xFF}},
        Fixture{"gray-alpha", 3, 16, 0x28, {0x22, 0x80, 0x66, 0x40}, {0x22, 0x22, 0x22, 0x80, 0x66, 0x66, 0x66, 0x40}},
    };

    for (auto const& fixture : fixtures)
    {
        SCOPED_TRACE(fixture.name);
        // Two adjacent pixels expose both incorrect lengths and incorrect channel strides.
        std::array<unsigned char, 18> header{};
        header[2]       = fixture.imageType;
        header[12]      = 2;
        header[14]      = 1;
        header[16]      = fixture.bitsPerPixel;
        header[17]      = fixture.descriptor;
        auto const path = directory / (std::string(fixture.name) + ".tga");
        {
            std::ofstream file(path, std::ios::binary);
            file.write(reinterpret_cast<char const*>(header.data()), static_cast<std::streamsize>(header.size()));
            file.write(
                reinterpret_cast<char const*>(fixture.source.data()),
                static_cast<std::streamsize>(fixture.source.size()));
        }

        auto image = polos::resource::image::LoadImage(path);
        ASSERT_NE(image, nullptr);
        EXPECT_EQ(image->width, 2);
        EXPECT_EQ(image->height, 1);
        EXPECT_EQ(image->components, 4);
        ASSERT_EQ(image->data.size(), fixture.expected.size());
        for (std::size_t i = 0; i < fixture.expected.size(); ++i)
            EXPECT_EQ(image->data[i], fixture.expected[i]) << "byte " << i;
    }
}

TEST_F(ImageLoaderTest, ReturnsNullForMissingOrMalformedImages)
{
    EXPECT_EQ(polos::resource::image::LoadImage(directory / "missing.png"), nullptr);

    auto path = directory / "invalid.png";
    {
        std::ofstream file(path);
        file << "not an image";
    }
    EXPECT_EQ(polos::resource::image::LoadImage(path), nullptr);
}

TEST(ResourceStorageTest, ReportsInsertionSuccessAndDuplicateKeys)
{
    polos::resource::Storage<int> storage;
    auto                          resource = std::make_shared<int>(42);
    EXPECT_TRUE(storage.Add("resource"_sid, resource));

    EXPECT_FALSE(storage.Add("resource"_sid, resource));
    EXPECT_EQ(storage.Get("resource"_sid), resource);
    EXPECT_EQ(storage.Get("missing"_sid), nullptr);
}

}// namespace
