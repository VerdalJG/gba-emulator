#pragma once
#include "Utils/Integer.hpp"

#include <utility>

class GBA_CPU;

enum class DataProcessingOpcode
{
    AND, EOR, SUB, RSB,
    ADD, ADC, SBC, RSC,
    TST, TEQ, CMP, CMN,
    ORR, MOV, BIC, MVN
};

using InstructionFunction = void (GBA_CPU::*)(uint32_t); // Instruction Function Pointer alias

struct Instruction
{
    u32 rawInstruction;
    InstructionFunction function = nullptr;
};

struct ShifterOperand 
{
    bool shifted = false;
    uint32_t value;
    uint32_t carryOut : 1; // Store only 1 bit of information
};

enum class InstructionCategory 
{
    DataProcessing,
    Multiply,
    LoadStore,
    Branch,
    Swap,
    SoftwareInterrupt,
    Coprocessor,
    Undefined
};

enum InstructionPattern : uint8_t 
{
    PATTERN_00, // Data Processing (immediate/register) and Misc: Multiply, Swap, Halfword transfer, Branch (BX)
    PATTERN_01, // Single Data Transfer or Undefined (bit 4 decides)
    PATTERN_10, // Block Data Transfer (LDM/STM) or Branch
    PATTERN_11, // Coprocessor or Software Interrupt
};


DataProcessingOpcode GetDataProcessingOpcode(uint32_t instruction);

inline bool CheckBits(uint32_t instruction, uint32_t shift, uint32_t mask, uint32_t expected)
{
    return ((instruction >> shift) & mask) == expected;
}

inline u32 SignExtend_8(u8 value)
{
    return (value & 0x80) ? (0xFFFFFF00 | static_cast<u32>(value)) : static_cast<u32>(value);
}

inline u32 SignExtend_16(u16 value)
{
    return (value & 0x8000) ? (0xFFFF0000 | static_cast<u32>(value)) : static_cast<u32>(value);
}

inline uint32_t CarryFrom(uint64_t result)
{
    return static_cast<uint32_t>(result >> 32);
}
