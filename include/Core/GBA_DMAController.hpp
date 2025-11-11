#pragma once  
#include <cstdint>

class EmulatorCore;

class GBA_DMAController
{
public:
    GBA_DMAController() = delete;
    explicit GBA_DMAController(EmulatorCore* core);
    ~GBA_DMAController() = default;

    void Step(uint32_t cycles);

private:
    EmulatorCore* core;
};