#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

using u8 = std::uint8_t;
using s8 = std::int8_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;
using u64 = std::uint64_t;
using s64 = std::int64_t;
using uint = unsigned int;

inline std::string IntToHexString(u32 address)
{
    std::ostringstream hex;
    hex << std::uppercase
        << std::hex
        << std::setw(8)
        << std::setfill('0')
        << address;

    return hex.str();
}