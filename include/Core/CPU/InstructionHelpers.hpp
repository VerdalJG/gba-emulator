#pragma once
#include "Utils/Integers.hpp"

#include <utility>

enum class ARM_ALUOp
{
    AND = 0, 
    EOR = 1, 
    SUB = 2, 
    RSB = 3,
    ADD = 4, 
    ADC = 5, 
    SBC = 6, 
    RSC = 7,
    TST = 8, 
    TEQ = 9, 
    CMP = 10, 
    CMN = 11,
    ORR = 12, 
    MOV = 13, 
    BIC = 14, 
    MVN = 15
};

enum class Thumb_ALUOp {
    AND = 0,
    EOR = 1,
    LSL = 2,
    LSR = 3,
    ASR = 4,
    ADC = 5,
    SBC = 6,
    ROR = 7,
    TST = 8,
    NEG = 9,
    CMP = 10,
    CMN = 11,
    ORR = 12,
    MUL = 13,
    BIC = 14,
    MVN = 15
};

enum InstructionPattern : uint8_t 
{
    PATTERN_00, // Data Processing (immediate/register) and Misc: Multiply, Swap, Halfword transfer, Branch (BX)
    PATTERN_01, // Single Data Transfer or Undefined (bit 4 decides)
    PATTERN_10, // Block Data Transfer (LDM/STM) or Branch
    PATTERN_11, // Coprocessor or Software Interrupt
};

u32 CalculateMultiplierCycles(u32 rsValue)
{
    if ((rsValue & 0xFFFFFF00) == 0x00000000 || (rsValue & 0xFFFFFF00) == 0xFFFFFF00)
    {
        return 1;
    } 

    if ((rsValue & 0xFFFF0000) == 0x00000000 || (rsValue & 0xFFFF0000) == 0xFFFF0000)
    {
        return 2;
    }

    if ((rsValue & 0xFF000000) == 0x00000000 || (rsValue & 0xFF000000) == 0xFF000000)
    {
        return 3;
    }

    return 4;
}