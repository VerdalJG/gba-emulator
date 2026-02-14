#pragma once
#include "Integer.hpp"

/// @brief Returns a range of bits
/// @tparam T integral type
/// @tparam hi Index of upper bit (inclusive)
/// @tparam lo Index of lower bit (inclusive)
/// @param value Value to extract bits from
/// @return An integral value, bits shifted down to least siginificant bits
template<int hi, int lo, typename T>
inline constexpr T ExtractBits(T value) 
{
    static_assert(hi < sizeof(T) * 4 && lo < sizeof(T) * 4);
    static_assert(hi >= lo && std::is_unsigned_v<T>);
    return (value >> lo) & ((T(1) << (hi - lo + 1)) - 1);
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