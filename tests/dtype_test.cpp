#include "tensor/dtype.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>

// Compile-time checks
static_assert(engine::dtype_size(engine::DType::Float32) == 4);
static_assert(engine::dtype_size(engine::DType::Int64) == 8);
static_assert(sizeof(engine::DType) == 1);

TEST(DTypeTest, SizeMatchesElementType)
{
    EXPECT_EQ(engine::dtype_size(engine::DType::Float32), sizeof(float));
    EXPECT_EQ(engine::dtype_size(engine::DType::Int64), sizeof(std::int64_t));
}

TEST(DTypeTest, UnknownValueThrows)
{
    const auto bad = static_cast<engine::DType>(99);
    EXPECT_THROW(engine::dtype_size(bad), std::invalid_argument);
}

TEST(DTypeTest, NameIsHumanReadable)
{
    EXPECT_EQ(engine::dtype_name(engine::DType::Float32), "float32");
    EXPECT_EQ(engine::dtype_name(engine::DType::Int64), "int64");
}