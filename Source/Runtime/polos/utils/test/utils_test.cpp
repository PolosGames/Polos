#include "polos/filesystem/filesystem_error_domain.hpp"
#include "polos/utils/constexpr_map.hpp"
#include "polos/utils/string_id.hpp"
#include "polos/utils/string_map.hpp"
#include "polos/utils/time.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace polos::utils
{
namespace
{
TEST(StringHashTest, MatchesKnownFnv1aVectors)
{
    struct HashCase
    {
        std::string_view input;
        std::uint64_t    expected;
    };

    constexpr std::array cases{
        HashCase{"", 0xcbf29ce484222325ULL},
        HashCase{"a", 0xaf63dc4c8601ec8cULL},
        HashCase{"foobar", 0x85944171f73967e8ULL},
    };

    for (auto const& testCase : cases)
    {
        SCOPED_TRACE(testCase.input);
        auto const actual = StrHash64(testCase.input);

        EXPECT_EQ(static_cast<std::uint64_t>(actual), testCase.expected);
    }
}

TEST(StringHashTest, HashesTheEntireViewIncludingEmbeddedNulls)
{
    constexpr std::string_view input{"a\0b", 3U};

    auto const fullHash = StrHash64(input);

    EXPECT_EQ(fullHash, "a\0b"_sid);
    EXPECT_NE(fullHash, "a"_sid);
    EXPECT_NE(fullHash, "ab"_sid);
}

TEST(StringRegistryTest, OwnsInputAndKeepsIdentityOnRepeatedRegistration)
{
    std::string input = "utils.registry.owned";
    auto const  id    = RegisterString(input);

    input.assign("overwritten source");
    auto const repeated = RegisterString("utils.registry.owned");

    EXPECT_EQ(id, "utils.registry.owned"_sid);
    EXPECT_EQ(repeated, id);
    EXPECT_EQ(GetStringFromID(id), "utils.registry.owned");
}

TEST(StringRegistryTest, ReturnedViewsRemainValidAfterMoreRegistrations)
{
    auto const  id           = RegisterString("utils.registry.stable-view");
    auto const  original     = GetStringFromID(id);
    auto const* originalData = original.data();

    for (unsigned i{0U}; i < 256U; ++i)
    {
        auto const registered = RegisterString("utils.registry.growth." + std::to_string(i));
        EXPECT_FALSE(GetStringFromID(registered).empty()) << i;
    }

    EXPECT_EQ(original, "utils.registry.stable-view");
    EXPECT_EQ(GetStringFromID(id).data(), originalData);
}

TEST(StringRegistryTest, UnknownIdsReturnAnEmptyView)
{
    auto const result = GetStringFromID("utils.registry.not-registered"_sid);

    EXPECT_TRUE(result.empty());
}

TEST(TimeConversionTest, ConvertsUnitsWithoutLosingIntegerPrecision)
{
    using namespace std::chrono;

    auto const millisecondsResult = ConvertToMicroseconds(milliseconds{1501});
    auto const secondsResult      = ConvertToMicroseconds(seconds{2});
    auto const minutesResult      = ConvertToMicroseconds(minutes{3});

    EXPECT_EQ(millisecondsResult.count(), 1'501'000);
    EXPECT_EQ(secondsResult.count(), 2'000'000);
    EXPECT_EQ(minutesResult.count(), 180'000'000);
    EXPECT_EQ(ConvertToMicroseconds(milliseconds{-2}).count(), -2'000);
    EXPECT_EQ(ConvertToMicroseconds(minutes{-1}).count(), -60'000'000);
    EXPECT_EQ(ConvertToMicroseconds(minutes{0}).count(), 0);
    EXPECT_EQ((2_ms).count(), 2'000);
    EXPECT_EQ((2_sec).count(), 2'000'000);
    EXPECT_EQ((3_min).count(), 180'000'000);
}

TEST(TimeConversionTest, ConvertsFractionalAndNegativeSeconds)
{
    struct ConversionCase
    {
        std::chrono::microseconds input;
        float                     expected;
    };

    constexpr std::array cases{
        ConversionCase{std::chrono::microseconds{0}, 0.0F},
        ConversionCase{std::chrono::microseconds{1'500'000}, 1.5F},
        ConversionCase{std::chrono::microseconds{-500'000}, -0.5F},
    };

    for (auto const& testCase : cases)
    {
        SCOPED_TRACE(testCase.input.count());
        auto const actual = ConvertToSeconds(testCase.input);

        EXPECT_NEAR(actual, testCase.expected, 0.000001F);
    }
}

TEST(ConstexprMapTest, FindsBoundaryEntriesAndReportsMissingKeys)
{
    constexpr std::array   entries{Pair{3, 30}, Pair{5, 50}, Pair{8, 80}};
    constexpr ConstexprMap map{entries};

    auto const first   = map.at(3);
    auto const last    = map.at(8);
    auto const missing = map.at(99);

    ASSERT_TRUE(first);
    ASSERT_TRUE(last);
    EXPECT_EQ(*first, 30);
    EXPECT_EQ(*last, 80);
    ASSERT_FALSE(missing);
    EXPECT_TRUE(missing.error() == MakeErrorCode(UtilsErrc::kValueNotFoundInMap));
}

TEST(ErrorCodeTest, EqualityDistinguishesDomainsAndCodes)
{
    auto const missingFile   = fs::MakeErrorCode(fs::FilesystemErrc::kFileWasNotFound);
    auto const sameFileError = fs::MakeErrorCode(fs::FilesystemErrc::kFileWasNotFound);
    auto const openFailure   = fs::MakeErrorCode(fs::FilesystemErrc::kFileOpenError);
    auto const missingValue  = MakeErrorCode(UtilsErrc::kValueNotFoundInMap);

    EXPECT_TRUE(missingFile == sameFileError);
    EXPECT_TRUE(missingFile != openFailure);
    EXPECT_EQ(missingFile.Code(), missingValue.Code());
    EXPECT_TRUE(missingFile != missingValue);
    EXPECT_EQ(missingFile.Domain().Name(), "Filesystem");
    EXPECT_EQ(missingFile.Message(), "Could not open file.");
    EXPECT_EQ(static_cast<std::string_view>(missingFile), missingFile.Message());
}

}// namespace
}// namespace polos::utils
