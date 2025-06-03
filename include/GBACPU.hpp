#pragma once

#include <cstdint>
#include <array>

class GBA_CPU 
{
public:
    GBA_CPU();
    ~GBA_CPU();
    std::array<uint32_t, 16> registers{}; // General-purpose registers
    uint32_t cpsr = 0;          // Current Program Status Register
    uint32_t spsr = 0;         // Saved Program Status Register
    bool thumb_mode;     // Thumb mode flag (0 = ARM mode, 1 = Thumb mode)

    void Reset();
    void Step();        // Fetch, decode, and execute loop
};

//https://problemkaputt.de/gbatek-arm-cpu-reference.htm - ARM CPU Reference
