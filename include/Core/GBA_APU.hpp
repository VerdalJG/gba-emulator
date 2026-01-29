#pragma once    
#include <cstdint>
#include "Core/GBA_IO_Helpers.hpp"

class EmulatorCore;
class GBA_Bus;

class GBA_APU
{
public:
    GBA_APU() = delete;
    explicit GBA_APU(EmulatorCore* core, GBA_Bus& bus, IO_SoundRegisters& soundRegisters);
    ~GBA_APU() = default;

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
    GBA_Bus& bus;
    IO_SoundRegisters& soundRegisters;
};