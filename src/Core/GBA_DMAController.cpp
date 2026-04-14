#include "Core/GBA_DMAController.hpp"
#include "Core/GBA_Bus.hpp"

#include <assert.h>

GBA_DMAController::GBA_DMAController(EmulatorCore *core, GBA_Bus& bus) : core(core), bus(bus)
{
    assert(core != nullptr && "DMA must have valid EmulatorCore object");
}

uint8_t GBA_DMAController::Read8_Bus(uint32_t address) { return 0; }
uint16_t GBA_DMAController::Read16_Bus(uint32_t address) { return 0; }
uint32_t GBA_DMAController::Read32_Bus(uint32_t address) { return 0; }
void GBA_DMAController::Step(uint32_t cycles) {}