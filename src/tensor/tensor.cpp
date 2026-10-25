#include "tensor/tensor.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <vector>

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

        std::int64_t numel_helper(const std::vector<std::int64_t>& shape)
        {
            std::int64_t numel = 1;
            
            
            for(auto dim : shape)
            {
                if(dim < 0)
                {
                    throw std::invalid_argument("shape cannot be negative");    
                }
                numel *= dim;
            } 
            
            return numel;
        }
        
    }

    Tensor::Tensor(std::vector<std::int64_t> shape, DType dtype) : shape_(std::move(shape)), dtype_(dtype)
    {

        

        numel_ = numel_helper(shape_);

        strides_ = strides_helper(shape_);

        storage_ = std::make_shared<Storage>(static_cast<std::size_t>(numel_) * dtype_size(dtype_));
        
         
    }

    Tensor::Tensor(std::shared_ptr<Storage> storage,
                   std::vector<std::int64_t> shape,
                   std::vector<std::int64_t> strides,
                   std::int64_t offset,
                   DType dtype) : shape_(std::move(shape)) ,strides_(std::move(strides)), offset_(offset), dtype_(dtype), storage_(std::move(storage)) 
                    {

                        numel_ = numel_helper(shape_);
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
    bool Tensor::is_contiguous() const noexcept
    {
        if(Tensor::numel_ == 0 || static_cast<std::int64_t>(shape_.size()) == 0)
        {
            return true;
        }
        std::vector<std::int64_t> expected_strides = strides_helper(Tensor::shape_);

        
        for(std::size_t d = 0; d < strides_.size(); d++)
        {
            if(shape_[d] == 1)
            {
                continue;
            }

            if(strides_[d] != expected_strides[d])
            {
                return false;
            }      

        }
        return true;
    }

    Tensor Tensor::view(std::vector<std::int64_t> new_shape) const
    {
        if(this -> is_contiguous() == false)
        {
            throw std::invalid_argument("tensor is not contiguous");
        }

        if(numel_helper(new_shape) != numel_)
        {
            throw std::invalid_argument("the numel of tensor is invalid");
        }

        std::vector<std::int64_t> new_strides = strides_helper(new_shape);

        return Tensor(storage_, std::move(new_shape), std::move(new_strides), offset_, dtype_);
    }

    Tensor Tensor::transpose(std::vector<std::int64_t> indexes) const
    {
        if(indexes.size() != 2)
        {
            throw std::invalid_argument("transpose can take two index parameters");
        }

        std::int64_t idx1 = indexes[0];
        std::int64_t idx2 = indexes[1];

        std::vector<std::int64_t> new_shape = shape_;
        std::vector<std::int64_t> new_strides = strides_;

        if(idx1 < 0 || idx2 < 0)
        {
            throw std::out_of_range("indexes cannot be negative");
        }

        if(idx1 < shape_.size() && idx2 < shape_.size())
        {
            std::swap(new_shape[idx1], new_shape[idx2]);
            std::swap(new_strides[idx1], new_strides[idx2]);
        }
        else
        {
            throw std::out_of_range("transpose: dimension index out of range");
        }

        return Tensor(storage_, std::move(new_shape), std::move(new_strides), offset_, dtype_);
    }

}