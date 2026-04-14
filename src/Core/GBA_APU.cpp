#include "Core/GBA_APU.hpp"
#include "Core/GBA_Bus.hpp"

#include <assert.h>

GBA_APU::GBA_APU(EmulatorCore *core, GBA_Bus& bus) : core(core),bus(bus)
{
    assert(core != nullptr && "APU must have valid EmulatorCore object");
}

void GBA_APU::Step(uint32_t cycles)
{

}