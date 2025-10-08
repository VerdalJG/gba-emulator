#pragma once
#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/CPU_Memory.hpp"

class TestCPU : public GBA_CPU
{
public:
    TestCPU(EmulatorCore* core) : GBA_CPU(core)
    {
        
    }

    uint32_t SetRegisterValue(int index, uint32_t value)
    {
        visibleRegisters[index] = value;
    }

    void SetCPSR(uint32_t flags)
    {
        cpsr = flags;
    }

};