#include "tensor/tensor.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <type_traits>


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

TEST(TensorTest, NegativeDimensionsThrows)
{

    EXPECT_THROW(engine::Tensor t({2,-3,4}, engine::DType::Int64), std::invalid_argument);
}

TEST(TensorTest, OneDimensionalTensor)
{
    engine::Tensor t({2}, engine::DType::Float32); 
    
    const std::vector<std::int64_t> expected_strides{1};

    EXPECT_EQ(t.rank(), 1);
    EXPECT_EQ(t.numel(), 2);
    EXPECT_EQ(t.strides(), expected_strides);
    EXPECT_EQ(t.nbytes(), 8u);
}

TEST(TensorTest, DataReadWriteFloat32)
{
    engine::Tensor t({2,3}, engine::DType::Float32);

    float* p = t.data<float>();
    ASSERT_NE(p, nullptr);

    p[0] = 2.0f;
    p[t.numel() - 1] = 32.0f;

    EXPECT_FLOAT_EQ(p[0], 2.0f);
    EXPECT_FLOAT_EQ(p[t.numel() - 1], 32.0f);


}

TEST(TensorTest, WrongTypeThrow)
{
    engine::Tensor t({2,2}, engine::DType::Int64);

    EXPECT_THROW(t.data<float>(), std::invalid_argument);

}

TEST(TensorTest, DataIsAligned)
{
    engine::Tensor t({3,2}, engine::DType::Float32);

    float* p = t.data<float>();
    auto addr = reinterpret_cast<std::uintptr_t>(p);
    EXPECT_EQ(addr % engine::Storage::kAlignment, 0u);
}

TEST(TensorTest, IsConstOverloadType)
{
    engine::Tensor t({4,2}, engine::DType::Float32);

    const engine::Tensor& ct = t;
    static_assert(std::is_same_v<decltype(ct.data<float>()), const float*>); 
}

TEST(TensorTest, TensorEmpty)
{
    engine::Tensor t = engine::Tensor::empty({2,3}, engine::DType::Float32);

    const std::vector<std::int64_t> expected_shape{2,3};

    EXPECT_EQ(t.shape(), expected_shape);
    EXPECT_EQ(t.numel(), 6);
    EXPECT_EQ(t.dtype(), engine::DType::Float32);
}

TEST(TensorTest, TensorZeros)
{
    engine::Tensor t = engine::Tensor::zeros({2,3}, engine::DType::Float32);

    float* p = t.data<float>();

    for(int i = 0; i < t.numel(); i++)
    {
        EXPECT_EQ(p[i], 0.0f);
    }
}

TEST(TensorTest, FromVector)
{
    std::vector<std::int64_t> values{1,2,3,4,5,6};

    engine::Tensor t = engine::Tensor::from_vector(values, {2,3});

    EXPECT_EQ(t.dtype(), engine::DType::Int64);

    const std::vector<std::int64_t> expected_shape{2,3};

    EXPECT_EQ(t.shape(), expected_shape);

    std::int64_t* p = t.data<std::int64_t>();

    for(int i = 0; i < t.numel(); i++)
    {
        EXPECT_EQ(p[i], values[i]);
    }
}

TEST(TensorTest, NewTensorIsContiguous)
{
    engine::Tensor t({2,3,4}, engine::DType::Float32);

    EXPECT_TRUE(t.is_contiguous());
}
TEST(TensorTest, ScalarIsContiguous)
{
    engine::Tensor t({}, engine::DType::Int64);

    EXPECT_TRUE(t.is_contiguous());
}
TEST(TensorTest, ZeroSizedIsContiguous)
{
    engine::Tensor t({2,0,3}, engine::DType::Float32);

    EXPECT_TRUE(t.is_contiguous());
}
TEST(TensorTest, SizeOneDimsAreContiguous)
{
    engine::Tensor t({1,3,1}, engine::DType::Float32);

    EXPECT_TRUE(t.is_contiguous());
}

TEST(TensorTest, ViewChangesShapeAndStrides)
{
    engine::Tensor t({2,3}, engine::DType::Float32);
    engine::Tensor v = t.view({3,2});

    const std::vector<std::int64_t> expected_shape{3,2};
    const std::vector<std::int64_t> expected_strides{2,1};

    EXPECT_EQ(v.shape(), expected_shape);
    EXPECT_EQ(v.strides(), expected_strides);
    EXPECT_EQ(v.numel(), 6);
}

TEST(TensorTest, ViewSharesStorage)
{
    engine::Tensor t = engine::Tensor::zeros({2,3}, engine::DType::Float32);
    engine::Tensor v = t.view({3,2});


    EXPECT_EQ(t.data<float>(), v.data<float>());

    float* vp = v.data<float>();
    vp[5] = 42.0f;

    const float* tp = t.data<float>();

    EXPECT_EQ(tp[5], 42.0f);

}

TEST(TensorTest, ViewNumelMismatchThrows)
{
    engine::Tensor t({2,3}, engine::DType::Float32);
    EXPECT_THROW(t.view({2,3,4}), std::invalid_argument);
}

TEST(TensorTest, TransposeOutOfRangeThrows)
{
    engine::Tensor t({2,3}, engine::DType::Float32);

    EXPECT_THROW(t.transpose({0,5}), std::out_of_range);
    EXPECT_THROW(t.transpose({-1,0}), std::out_of_range);
}

TEST(TensorTest, TransposeWork)
{
    std::vector<float> values{1,2,3,4,5,6};
    engine::Tensor t = engine::Tensor::from_vector(values, {2,3});
    engine::Tensor tt = t.transpose({0,1});
    
    const float* p = tt.data<float>();
    const auto& s = tt.strides();

    EXPECT_EQ(p[0 * s[0] + 1 * s[1]], 4.0f);
}

TEST(TensorTest, ContiguousCopiesInLogicalOrder)
{
    std::vector<float> values{1,2,3,4,5,6};
    engine::Tensor t = engine::Tensor::from_vector(values, {2,3});
    engine::Tensor c = t.transpose({0,1}).contiguous();

    const std::vector<float> expected{1,4,2,5,3,6};
    const float* p = c.data<float>();

    for(size_t i= 0; i < values.size(); i++)
    {
        EXPECT_EQ(p[i], expected[i]);
    }
    EXPECT_TRUE(c.is_contiguous());
}

TEST(TensorTest, ContiguousCopiesInt64)
{
    std::vector<std::int64_t> values{1,2,3,4,5,6};
    engine::Tensor t = engine::Tensor::from_vector(values, {2,3});
    engine::Tensor c = t.transpose({0,1}).contiguous();

    const std::vector<std::int64_t> expected{1,4,2,5,3,6};
    const std::int64_t* p = c.data<std::int64_t>();

    for(std::size_t i = 0; i < expected.size(); i++)
    {
        EXPECT_EQ(p[i], expected[i]);
    }
    EXPECT_TRUE(c.is_contiguous());
}

TEST(TensorTest, ContiguousThreeDimensional)
{
    std::vector<float> values{0,1,2,3,4,5,6,7,8,9,10,11};
    engine::Tensor t = engine::Tensor::from_vector(values, {2,2,3});
    engine::Tensor c = t.transpose({0,2}).contiguous();

    // c(a,b,k) == t(k,b,a) == k*6 + b*3 + a
    const std::vector<float> expected{0,6,3,9, 1,7,4,10, 2,8,5,11};
    const float* p = c.data<float>();

    const std::vector<std::int64_t> expected_shape{3,2,2};
    EXPECT_EQ(c.shape(), expected_shape);
    for(std::size_t i = 0; i < expected.size(); i++)
    {
        EXPECT_EQ(p[i], expected[i]);
    }
    EXPECT_TRUE(c.is_contiguous());
}

TEST(TensorTest, ContiguousOnContiguousSharesStorage)
{
    engine::Tensor t = engine::Tensor::zeros({2,3}, engine::DType::Float32);
    engine::Tensor c = t.contiguous();

    EXPECT_EQ(c.data<float>(), t.data<float>());
}


