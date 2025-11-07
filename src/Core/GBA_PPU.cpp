#include "Core/GBA_PPU.hpp"
#include <assert.h>

GBA_PPU::GBA_PPU(EmulatorCore *core) : core(core)
{
    assert(core != nullptr && "PPU must have valid EmulatorCore object");
}

void GBA_PPU::Step(uint32_t cycles)
{
}
