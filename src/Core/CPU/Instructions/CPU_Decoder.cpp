#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/Instructions/CPU_Shifts.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"
#include "Core/CPU/Instructions/CPU_DataProcessing.hpp"

InstructionFunction DecodePattern00(uint32_t instruction, GBA_CPU& cpu)
{
    bool bit25 = (instruction >> 25) & 1;

    // Data processing
    if (bit25)
    {
        return &DataProcessing;
    }

    bool bit4 = (instruction >> 4) & 1;
    bool bit7 = (instruction >> 7) & 1;
    if (bit4 && bit7) // Misc
    {
        bool bit5 = (instruction >> 5) & 1;
        bool bit6 = (instruction >> 6) & 1;
        bool mulUpperBits = ((instruction >> 22) & 0b111) == 0b000;
        if (mulUpperBits && !bit5 && !bit6)
        {
            return &Multiply;
        }

        bool mullUpperBits = ((instruction >> 23) & 0b11) == 0b01;
        if (mullUpperBits && !bit5 && !bit6)
        {
            return &MultiplyLong;
        }

        bool sdsUpperBits = ((instruction >> 20) & 0b11011) == 0b10000;
        bool sdsLowerBits = ((instruction >> 8) & 0xF) == 0x0;
        if (sdsUpperBits && sdsLowerBits)
        {
            return &SingleDataSwap;
        }
 
        return &HalfwordDataTransfer;
    }

    // Can only be BX and Data Processing with Shifted Register at this point
    bool bxPattern = ((instruction >> 4) & 0x1FFFFF) == 0x12FFF1;
    if (bxPattern)
    {
        return &BranchAndExchange;
    }

    // Fallback to data processing
    return &DataProcessing;
}

InstructionFunction DecodePattern01(uint32_t instruction, GBA_CPU& cpu)
{
    if ((instruction >> 4) & 1)
    {
        return &UndefinedInstruction;
    }
    else 
    {
        return &SingleDataTransfer;
    }
}

InstructionFunction DecodePattern10(uint32_t instruction, GBA_CPU& cpu)
{
    return InstructionFunction();
}

InstructionFunction DecodePattern11(uint32_t instruction, GBA_CPU& cpu)
{
    return InstructionFunction();
}


