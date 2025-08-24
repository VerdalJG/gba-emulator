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
        /*
            Multiply,
            MultiplyLong,
            Single Data Swap,
            Branch and exchange,
            Halfword Data Transfer register offset,
            Halfword Data Transfer immediate offset,        
        */
    }

    // Data processing with shifts (immediate or register)
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