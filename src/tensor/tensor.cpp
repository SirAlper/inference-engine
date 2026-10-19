#include "tensor/tensor.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace engine
{
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

        strides_.resize(shape_.size());
        
        std::int64_t running = 1;
        std::int64_t border = 0;
        
    
        std::int64_t rank = static_cast<std::int64_t>(shape_.size());
        
        for(std::int64_t i= rank - 1; border <= i; i--)
        {   
            strides_[i] = running;
            running *= std::max(shape_[i], std::int64_t{1});                
        }

        storage_ = std::make_shared<Storage>(static_cast<std::size_t>(numel_) * dtype_size(dtype_));

         
    }
    

}