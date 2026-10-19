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

    template <typename T>
    struct dtype_of;

    template<>
    struct dtype_of<float> {static constexpr DType value = DType::Float32;};

    template<>
    struct dtype_of<std::int64_t>{static constexpr DType value = DType::Int64;};

    template<typename T>
    inline constexpr DType dtype_of_v = dtype_of<T>::value; 
    
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