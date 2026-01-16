#pragma once
#include <cstdint>

enum class BusAccessSize
{
    Byte = 1,
    Halfword = 2,
    Word = 4
};

class GBA_Memory;
class EmulatorCore;

class GBA_Bus
{
public:
    GBA_Bus(EmulatorCore* core, GBA_Memory& memory);

    GBA_MemoryRegionType GetRegionFromAddress(uint32_t address) const;

    uint8_t Read8(uint32_t address, uint32_t& cycles);
    uint16_t Read16(uint32_t address, uint32_t& cycles);
    uint32_t Read32(uint32_t address, uint32_t& cycles);
    
    void Write(uint32_t address, uint32_t value, BusAccessSize size);

    // Open bus tracking
    uint32_t GetLastValue() const { return lastValue; }
    BusAccessSize GetLastAccessSize() const { return lastSize; }

    private:
    // Last value read from the bus (for open bus behavior)
    uint32_t lastValue = 0;
    BusAccessSize lastSize = BusAccessSize::Word;
    
    EmulatorCore* core;
    GBA_Memory& memory;
};