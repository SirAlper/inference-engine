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
            
            return Tensor(storage_, new_shape, new_strides, offset_, dtype_);;
        }
    

}