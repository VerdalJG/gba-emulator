#include "Core/GBA_DMAController.hpp"

#include <assert.h>

GBA_DMAController::GBA_DMAController(EmulatorCore *core) : core(core)
{
    assert(core != nullptr && "DMA must have valid EmulatorCore object");
}

void GBA_DMAController::Step(uint32_t cycles)
{
}