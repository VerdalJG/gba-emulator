#pragma once  
#include <cstdint>

class EmulatorCore;

class GBA_InterruptController
{
public:
    GBA_InterruptController() = delete;
    explicit GBA_InterruptController(EmulatorCore* core);
    ~GBA_InterruptController() = default;

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
};