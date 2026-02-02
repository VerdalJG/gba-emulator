#include "Core/CPU/Instructions/Decoder.hpp"
#include "Core/CPU/Instructions/Shifts.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"
#include "Core/CPU/Instructions/DataProcessing.hpp"
#include "Core/CPU/Instructions/DataProcessingMisc.hpp"
#include "Core/CPU/Instructions/LoadStore.hpp"
#include "Core/CPU/Instructions/Exceptions.hpp"
#include "Core/CPU/Instructions/Branches.hpp"
#include "Core/GBA_CPU.hpp"

InstructionFunction DecodeInstruction(uint32_t instruction, GBA_CPU& cpu)
{
    InstructionPattern pattern = static_cast<InstructionPattern>((instruction >> 26) & 0b11);
    switch (pattern)
    {
        case PATTERN_00: // Data processing and misc
        return DecodePattern00(instruction, cpu);

        case PATTERN_01: // Single Data Transfer or Undefined (bit 4 decides)
        return DecodePattern01(instruction, cpu); 

        case PATTERN_10: // Block Data Transfer (LDM/STM) or Branch
        return DecodePattern10(instruction, cpu);

        case PATTERN_11: // Coprocessor or Software Interrupt
        return DecodePattern11(instruction, cpu);

        default: // In theory is unreachable
        //HandleUndefinedBehavior(instruction, *this); - Normally something like this would be called
        return nullptr;
    }
}

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
    if ((instruction >> 25) & 1)
    {
        return &Branch;
    }
    else
    {
        return &BlockDataTransfer;
    }
}

InstructionFunction DecodePattern11(uint32_t instruction, GBA_CPU& cpu)
{
    if ((instruction >> 25) & 1)
    {
        if ((instruction >> 24) & 1)
        {
            &SoftwareInterrupt;
        }
    }

    /*
        Normally anything else would be a coprocessor function:
        Coprocessor register transfer
        Coprocessor data operation
        Coprocessor data transfer
    */ 
   
    // Because GBA only implements SWI, no coprocessor operations
    return &UndefinedInstruction; 
}


