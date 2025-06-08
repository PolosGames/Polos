//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/memory/debug_allocator.hpp"

#include "polos/logging/log_macros.hpp"

#include <gtest/gtest.h>

#include <memory_resource>
#include <vector>

namespace
{

using polos::memory::DebugAllocator;

TEST(DebugAllocatorTestFixture, AllocateOnVectorTest)
{
    DebugAllocator        allocator{"TestAllocator", std::pmr::new_delete_resource()};
    std::pmr::vector<int> int_vec({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, allocator.GetMemoryResource());

    EXPECT_EQ(allocator.GetUsedMemoryInBytes(), 56);// 40 + 16 (vector internal structures)
    int_vec.push_back(11);
    // 60 (increased by (int_vec.size() / 2) * sizeof(int) ) + 16 (vector internal structures)
    EXPECT_EQ(allocator.GetUsedMemoryInBytes(), 76);
    int_vec.push_back(12);
    EXPECT_EQ(allocator.GetUsedMemoryInBytes(), 76);// Shouldn't change, enough room for 5 more int's
}

}// namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
