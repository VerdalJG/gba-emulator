#pragma once  
#include <cstdint>
#include "Core/GBA_IO_Helpers.hpp"

class EmulatorCore;

class GBA_DMAController
{
public:
    GBA_DMAController() = delete;
    explicit GBA_DMAController(EmulatorCore* core, IO_DMARegisters& dmaRegisters);
    ~GBA_DMAController() = default;

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
    IO_DMARegisters& dmaRegisters;
};