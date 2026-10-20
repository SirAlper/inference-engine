#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

#include "dtype.hpp"
#include "storage.hpp"

namespace engine
{
    class Tensor
    {
        public:
            explicit Tensor(std::vector<std::int64_t> shape, DType dtype);

            static Tensor empty(std::vector<std::int64_t> shape, DType dtype);
            static Tensor zeros(std::vector<std::int64_t> shape, DType dtype);

            const std::vector<std::int64_t>& shape()   const noexcept { return shape_; }
            const std::vector<std::int64_t>& strides() const noexcept { return strides_; }
            DType                            dtype()   const noexcept { return dtype_; }
            std::int64_t                     rank()    const noexcept { return static_cast<std::int64_t>(shape_.size()); }
            std::int64_t                     numel()   const noexcept { return numel_; }
            std::int64_t                     offset()  const noexcept { return offset_; }
            std::size_t                      nbytes()  const noexcept { return static_cast<std::size_t>(numel_) * dtype_size(dtype_); }
            
            template <typename U>
            U* data() 
            {
                if(dtype_of_v<U> != dtype_)
                {
                    throw std::invalid_argument("invalid argument of dtype");
                }

                std::byte* raw = storage_->data();
                U* typed = reinterpret_cast<U*>(raw);

                return typed + offset_;
                

            }

            template <typename U>
            const U* data() const 
            {
                if(dtype_of_v<U> != dtype_)
                {
                    throw std::invalid_argument("invalid argument of dtype");
                }
                const std::byte* raw = storage_ -> data();
                const U* typed = reinterpret_cast<const U*>(raw);

                return typed + offset_;

            }

        private:
            std::vector<std::int64_t> shape_;
            std::vector<std::int64_t> strides_;
            std::int64_t offset_ = 0;
            std::int64_t numel_ = 0;
            DType dtype_;
            std::shared_ptr<Storage> storage_;
    };
}