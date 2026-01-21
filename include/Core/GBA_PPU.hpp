#pragma once    
#include <cstdint>
#include "Core/GBA_IO_Helpers.hpp"

class EmulatorCore;

class GBA_PPU
{
public:
    GBA_PPU() = delete;
    explicit GBA_PPU(EmulatorCore* core, IO_LCDRegisters& lcdRegisters);
    ~GBA_PPU() = default;

    bool FrameReady();

    void Step(uint32_t cycles);

    uint16_t Read16(uint32_t address);

    void Write16(uint32_t address, uint16_t value);

private:
    EmulatorCore* core;
    IO_LCDRegisters& lcdRegisters;
};