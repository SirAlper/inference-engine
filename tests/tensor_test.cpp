#include "tensor/tensor.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <vector>



TEST(TensorTest, BasicProperties)
{
    engine::Tensor t({2,3,4,}, engine::DType::Float32);

    const std::vector<std::int64_t> expected_shape{2,3,4};
    EXPECT_EQ(t.shape(), expected_shape);
    EXPECT_EQ(t.dtype(), engine::DType::Float32);
    EXPECT_EQ(t.numel(), 24);
    EXPECT_EQ(t.nbytes(), 96u);

}


TEST(TensorTest, StridesAreRowMajor)
{
    engine::Tensor t({2,3,4}, engine::DType::Float32);

    const std::vector<std::int64_t> expected_strides{12,4,1};
    EXPECT_EQ(t.strides(), expected_strides);
}