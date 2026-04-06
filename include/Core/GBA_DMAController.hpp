#pragma once  
#include "Core/IO/GBA_IO_Helpers.hpp"

#include <cstdint>

class EmulatorCore;
class GBA_Bus;

class GBA_DMAController
{
public:
    GBA_DMAController() = delete;
    explicit GBA_DMAController(EmulatorCore* core, GBA_Bus& bus);
    ~GBA_DMAController() = default;

    uint8_t Read8_Bus(uint32_t address);
    uint16_t Read16_Bus(uint32_t address);
    uint32_t Read32_Bus(uint32_t address);

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
    GBA_Bus& bus;
    IO_DMARegisters& dmaRegisters;
};