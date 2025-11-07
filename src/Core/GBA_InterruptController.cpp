#include "Core/GBA_InterruptController.hpp"
#include <assert.h>

GBA_InterruptController::GBA_InterruptController(EmulatorCore *core) : core(core)
{
    assert(core != nullptr && "APU must have valid EmulatorCore object");
}

void GBA_InterruptController::Step(uint32_t cycles)
{

}