#include "tensor/storage.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

TEST(StorageTest, DataIsAligned)
{
    engine::Storage s(1024);
    ASSERT_NE(s.data(), nullptr);
    auto addr = reinterpret_cast<std::uintptr_t>(s.data());
    EXPECT_EQ(addr % engine::Storage::kAlignment, 0u);
}

TEST(StorageTest, ReportRequestedSize)
{
    engine::Storage s(1024);
    EXPECT_EQ(s.nbytes(), 1024u);
}

TEST(StorageTest, ZeroBytesHasNullData)
{
    engine::Storage s(0);
    EXPECT_EQ(s.nbytes(), 0u);
    EXPECT_EQ(s.data(), nullptr);
}

TEST(StorageTest, DataIsAlignedForVariousSizes)
{
    const std::size_t sizes[] = {1, 3, 63, 64, 65, 4096};
    for (std::size_t n : sizes)
    {
        engine::Storage s(n);
        ASSERT_NE(s.data(), nullptr) << "size " << n;
        auto addr = reinterpret_cast<std::uintptr_t>(s.data());
        EXPECT_EQ(addr % engine::Storage::kAlignment, 0u) << "size " << n;
    }
}
