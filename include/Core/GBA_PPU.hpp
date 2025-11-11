#pragma once    
#include <cstdint>

class EmulatorCore;

class GBA_PPU
{
public:
    GBA_PPU() = delete;
    explicit GBA_PPU(EmulatorCore* core);
    ~GBA_PPU() = default;

    bool FrameReady();

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
};