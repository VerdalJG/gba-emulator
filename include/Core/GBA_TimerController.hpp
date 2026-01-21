#pragma once
#include <cstdint>
#include "Core/GBA_IO_Helpers.hpp"

class EmulatorCore;

class GBA_TimerController
{
public:
    GBA_TimerController() = delete;
    explicit GBA_TimerController(EmulatorCore* core, IO_TimerRegisters& timerRegisters);
    ~GBA_TimerController() = default;

    void AddCycles(uint32_t cycles);

    uint64_t GetTotalCycles() const { return totalCycles; }

private:
    EmulatorCore* core;
    IO_TimerRegisters& timerRegisters;
    uint64_t totalCycles = 0;
};