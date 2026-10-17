#pragma once
#include <cstddef>

namespace engine
{
    // Owns a single 64-byte aligned, uninitialized byte buffer.
    // Non-copyable and non-movable; lifetime is tied to the owning object.
    class Storage
    {
    public:
        static constexpr std::size_t kAlignment = 64;
        static_assert((kAlignment & (kAlignment - 1)) == 0, "kAlignment must be a power of two");

        explicit Storage(std::size_t nbytes);
        ~Storage();

        Storage(const Storage&) = delete;
        Storage& operator=(const Storage&) = delete;
        Storage(Storage&&) = delete;
        Storage& operator=(Storage&&) = delete;

        std::size_t      nbytes() const noexcept { return nbytes_; }
        std::byte*       data()         noexcept { return data_; }
        const std::byte* data()   const noexcept { return data_; }

    private:
        std::size_t nbytes_ = 0;
        std::byte*  data_   = nullptr;
    };
} // namespace engine
