#pragma once    
#include <cstdint>

class EmulatorCore;

class GBA_APU
{
public:
    GBA_APU() = delete;
    explicit GBA_APU(EmulatorCore* core);
    ~GBA_APU() = default;

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
};