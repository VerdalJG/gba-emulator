#pragma once
#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/CPU_Memory.hpp"

class TestCPU : public GBA_CPU
{
public:
    TestCPU(GBA_Memory& memory) : GBA_CPU(memory)
    {
        
    }

    uint32_t SetRegisterValue(int index, uint32_t value)
    {
        registers[index] = value;
    }

    void SetCPSR(uint32_t flags)
    {
        cpsr = flags;
    }

};