#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>


namespace engine
{
    enum class DType : std::uint8_t
    {
        Float32,
        Int64
    };
    
    constexpr std::size_t dtype_size(DType dtype)
    {
        switch(dtype)
        {
            case DType::Float32:
                return sizeof(float);
            case DType::Int64:
                return sizeof(std::int64_t);    
        }

        throw std::invalid_argument("dtype_size: unknown DType");
    }

    constexpr std::string_view dtype_name(DType dtype)
    {
        switch(dtype)
        {
            case DType::Float32:
                return "float32";
            case DType::Int64:
                return "int64";
        }
        throw std::invalid_argument("dtype_name: unknown DType");
    }
    
}// namespace engine