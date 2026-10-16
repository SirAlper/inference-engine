#pragma once
#include <cstddef>

class Storage
{
    private:

        std::size_t nbytes_ = 0;
        std::byte* data_ = nullptr;

    public:
        
        explicit Storage(std::size_t nbytes);
        Storage(const Storage&) = delete;
        static constexpr std::size_t kAlignment = 64;
        Storage& operator=(const Storage&) = delete;
        ~Storage();
        std::size_t nbytes() const;
        std::byte* data() const;
        
};
