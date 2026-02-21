#pragma once
#include "Integers.hpp"

/// @brief Returns a range of bits
/// @tparam hi Index of upper bit (inclusive)
/// @tparam lo Index of lower bit (inclusive)
/// @param value Value to extract bits from
/// @return An integral value, bits shifted down to least siginificant bits
template<int hi, int lo>
inline constexpr u32 ExtractBits(u32 value) 
{
    static_assert(hi < 32 && lo < 31);
    static_assert(hi > lo);
    return (value >> lo) & ((static_cast<u32>(1) << (hi - lo + 1)) - 1);
}


/// @brief Returns a range of bits
/// @tparam hi Index of upper bit (inclusive)
/// @tparam lo Index of lower bit (inclusive)
/// @param value Value to extract bits from
/// @return An integral value, bits shifted down to least siginificant bits
template<int hi, int lo>
inline constexpr u16 ExtractBits(u16 value) 
{
    static_assert(hi < 16 && lo < 15);
    static_assert(hi > lo);
    return (value >> lo) & ((static_cast<u16>(1) << (hi - lo + 1)) - 1);
}

template<int bitIndex>
inline constexpr bool IsBitSet(u32 value)
{
    static_assert(bitIndex < sizeof(u32) * 4);
    return (value >> bitIndex) & 1;
}

inline uint32_t NumberOfSetBitsIn(uint32_t value)
{
    uint32_t result = 0;
    while (value != 0)
    {
        value &= (value - 1);
        result++;
    }

    return result;
}

inline constexpr u32 SignExtend_8(u8 value)
{
    return (value & 0x80) ? (0xFFFFFF00 | static_cast<u32>(value)) : static_cast<u32>(value);
}

inline constexpr u32 SignExtend_16(u16 value)
{
    return (value & 0x8000) ? (0xFFFF0000 | static_cast<u32>(value)) : static_cast<u32>(value);
}
