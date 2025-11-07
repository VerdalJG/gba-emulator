#include "Core/GBA_TimerController.hpp"
#include "Core/EmulatorCore.hpp"

#include <assert.h>

GBA_TimerController::GBA_TimerController(EmulatorCore *core) : core(core)
{
    assert(core != nullptr && "TimerController must have valid EmulatorCore object");
}

void GBA_TimerController::AddCycles(uint32_t cycles)
{
    if (!core || cycles == 0) return;

    totalCycles += cycles;
    core->Step(cycles);
}
