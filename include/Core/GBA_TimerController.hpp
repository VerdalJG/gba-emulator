#pragma once
#include <cstdint>

class EmulatorCore;

class GBA_TimerController
{
public:
    GBA_TimerController() = delete;
    explicit GBA_TimerController(EmulatorCore* core);
    ~GBA_TimerController() = default;

    void AddCycles(uint32_t cycles);

    uint64_t GetTotalCycles() const { return totalCycles; }

private:
    EmulatorCore* core;
    uint64_t totalCycles = 0;
};