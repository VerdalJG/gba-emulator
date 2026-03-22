#include "Utils/Integers.hpp"

// Address already aligned/mirroring accounted for, no need for defensive checks at this point
template<typename T>
T GBA_Memory::Read(u32 address)
{
    const MemoryRegion* region = GetRegionFromAddress(address);
    u32 offset = address - region->start;
    std::span<const uint8_t> regionData = GetRegionData(region->type);

    T readValue = 0;

    for (size_t i = 0; i < sizeof(T); i++)
    {
        readValue |= static_cast<T>(regionData[offset + i]) << (8 * i);
    }
    return readValue;
}

// Address already aligned/mirroring accounted for
template<typename T>
void GBA_Memory::Write(u32 address, T value)
{
    const MemoryRegion* region = GetRegionFromAddress(address);
    u32 offset = address - region->start;
    std::span<uint8_t> regionData = GetRegionDataMutable(region->type);

    for (size_t i = 0; i < sizeof(T); i++)
    {
        regionData[offset + i] = static_cast<u8>((value >> (8 * i)) & 0xFF);
    }
}

