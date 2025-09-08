#include "Core/InstructionHelpers.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/CPU_Shifts.hpp"

GBA_CPU::InstructionFunction GBA_CPU::DecodePattern00(uint32_t instruction)
{
    bool bit25 = (instruction >> 25) & 1;

    // Data processing with 32-bit immediate value
    if (bit25)
    {
        DataProcessingOpcode opcode = GetDataProcessingOpcode(instruction);
        return dataProcessingFuncTable[static_cast<int>(opcode)];
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

    // Data processing with shift (immediate or register)
    DataProcessingOpcode opcode = GetDataProcessingOpcode(instruction);
    return dataProcessingFuncTable[static_cast<int>(opcode)];
}

GBA_CPU::InstructionFunction GBA_CPU::DecodePattern01(uint32_t instruction)
{
    return InstructionFunction();
}

GBA_CPU::InstructionFunction GBA_CPU::DecodePattern10(uint32_t instruction)
{
    return InstructionFunction();
}

GBA_CPU::InstructionFunction GBA_CPU::DecodePattern11(uint32_t instruction)
{
    return InstructionFunction();
}

void GBA_CPU::HandleUndefinedBehavior(uint32_t instruction)
{
}
