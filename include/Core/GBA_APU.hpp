#pragma once    
#include <cstdint>
#include "Core/GBA_IO_Helpers.hpp"

class EmulatorCore;

class GBA_APU
{
public:
    GBA_APU() = delete;
    explicit GBA_APU(EmulatorCore* core, IO_SoundRegisters& soundRegisters);
    ~GBA_APU() = default;

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
    IO_SoundRegisters& soundRegisters;
};