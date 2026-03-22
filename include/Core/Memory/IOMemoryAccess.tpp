#include "Utils/Integers.hpp"

template <typename T>
T GBA_IO::Read(u32 address)
{
    AccessSize access = sizeof(T);
    switch (access)
    {
        case AccessSize::Byte: return Read8(address);
        case AccessSize::Halfword: return Read16(address);
        case AccessSize::Word: return Read32(address);
    }
}

template <typename T>
void GBA_IO::Write(u32 address, T value)
{
    AccessSize access = sizeof(T);
    switch (access)
    {
        case AccessSize::Byte: return Write8(address, value);
        case AccessSize::Halfword: return Write16(address, value);
        case AccessSize::Word: return Write32(address, value);
    }
}