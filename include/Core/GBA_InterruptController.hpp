#pragma once  
#include <cstdint>
#include "GBA_IO_Helpers.hpp"

class EmulatorCore;

class GBA_InterruptController
{
public:
    GBA_InterruptController() = delete;
    explicit GBA_InterruptController(EmulatorCore* core, IO_InterruptRegisters& interruptRegisters);
    ~GBA_InterruptController() = default;

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
    IO_InterruptRegisters& interruptRegisters;
};