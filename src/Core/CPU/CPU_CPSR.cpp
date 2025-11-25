#include "Core/GBA_CPU.hpp"
#include "Core/CPU/CPU_CPSR.hpp"
#include "Core/CPU/CPU_Timings.hpp"

uint32_t CPSR_OverflowFrom(uint32_t op1, uint32_t op2, uint32_t result, bool isSub)
{
    if (isSub)
    {
        // SUB / SBC
        bool operandsDifferentSign = ((op1 ^ op2) & 0x80000000) != 0;
        bool resultDifferentSign = ((result ^ op1) & 0x80000000) != 0;
        return operandsDifferentSign && resultDifferentSign ? (1 << 28) : 0;
    }
    else
    {
        // ADD / ADC
        bool operandsSameSign = ((op1 ^ op2) & 0x80000000) == 0;
        bool resultDifferentSign = ((result ^ op1) & 0x80000000) != 0;
        return operandsSameSign && resultDifferentSign ? (1 << 28) : 0;
    }
}

void HandleProgramCounterCPSRCase(GBA_CPU& cpu)
{
    if (cpu.CurrentModeHasSPSR())
    {
        int exceptionModeIndex = BankIndex(cpu.GetCurrentOperatingMode());
        OperatingMode oldMode = cpu.GetCurrentOperatingMode();
        cpu.RestoreCPSRFromSPSR(exceptionModeIndex);
        OperatingMode newMode = cpu.GetCurrentOperatingMode();

        if (oldMode != newMode)
        {
            cpu.AddCycles(CPU_Timings::MODE_SWITCH_PENALTY);
        }
    }
    else
    {
        // Normally UNPREDICTABLE in other ARM versions, GBA treats it as no-op
    }
}