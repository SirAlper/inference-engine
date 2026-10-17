#include "tensor/storage.hpp"
#include <new>

namespace engine
{
    Storage::Storage(std::size_t nbytes) : nbytes_(nbytes)
    {
        if (nbytes_ != 0){
            data_ = static_cast<std::byte*>(::operator new(nbytes_, std::align_val_t{kAlignment}));     
        }
        


    }

    Storage::~Storage()
    {
        ::operator delete(data_, std::align_val_t{kAlignment});
    }
}// namespace engine