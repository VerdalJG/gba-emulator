#include "Core/GBA_APU.hpp"
#include <assert.h>

GBA_APU::GBA_APU(EmulatorCore *core) : core(core)
{
    assert(core != nullptr && "APU must have valid EmulatorCore object");
}

void GBA_APU::Step(uint32_t cycles)
{

}