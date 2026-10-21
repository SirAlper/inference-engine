#include "tensor/tensor.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <cstring>

namespace engine
{
    namespace
    {
        std::vector<int64_t> strides_helper(const std::vector<int64_t>& shape)
        {
            std::vector<int64_t> strides;

            strides.resize(shape.size());

            std::int64_t running = 1;

            std::int64_t rank = static_cast<std::int64_t>(shape.size());

            for(std::int64_t i = rank - 1;  0 <= i; i--)
            {
                strides[i] = running;
                running *= std::max(shape[i], std::int64_t{1});
            }

            return strides;
        }
    }

    Tensor::Tensor(std::vector<std::int64_t> shape, DType dtype) : shape_(std::move(shape)), dtype_(dtype)
    {

        

        std::int64_t tempnumel = 1;
        for(auto dim : shape_)
        {
            if(dim < 0)
            {
                throw std::invalid_argument("shape cannot be negative");
            }
            tempnumel *= dim;
        }
        numel_ = tempnumel;

        strides_ = strides_helper(shape_);

        storage_ = std::make_shared<Storage>(static_cast<std::size_t>(numel_) * dtype_size(dtype_));
        
         
    }

    Tensor Tensor::empty(std::vector<std::int64_t> shape, DType dtype)
    {
        return Tensor(std::move(shape), dtype);       
    }

    Tensor Tensor::zeros(std::vector<std::int64_t> shape, DType dtype)
    {
        Tensor t = Tensor::empty(std::move(shape), dtype);

        if(t.nbytes() > 0)
        {
            std::memset(t.storage_ -> data(), 0, t.nbytes());
        }
        return t;
    }
    

}