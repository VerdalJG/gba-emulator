#pragma once    
#include <cstdint>
#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Core/IO/Registers/SoundRegisters.hpp"

class EmulatorCore;
class GBA_Bus;

class GBA_APU
{
public:
    GBA_APU() = delete;
    explicit GBA_APU(EmulatorCore* core, GBA_Bus& bus);
    ~GBA_APU() = default;

    void Step(uint32_t cycles);
    IO_SoundRegisters& GetSoundRegisters() { return soundRegisters; }

private:
    EmulatorCore* core;
    GBA_Bus& bus;
    IO_SoundRegisters soundRegisters;
};