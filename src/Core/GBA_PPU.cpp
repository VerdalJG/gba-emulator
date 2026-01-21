#include "Core/GBA_PPU.hpp"
#include <assert.h>

GBA_PPU::GBA_PPU(EmulatorCore *core, IO_LCDRegisters& lcdRegisters) : core(core), 
    lcdRegisters(lcdRegisters)
{
    assert(core != nullptr && "PPU must have valid EmulatorCore object");
}

bool GBA_PPU::FrameReady()
{
    return false;
}

void GBA_PPU::Step(uint32_t cycles) 
{
}

uint16_t GBA_PPU::Read16(uint32_t address) 
{ 
    return 0; 
}

void GBA_PPU::Write16(uint32_t address, uint16_t value) 
{ 
    return; 
}
