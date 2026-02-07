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