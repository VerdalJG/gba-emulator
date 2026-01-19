#pragma once
#include <cstdint>
#include <utility>


class GBA_CPU;

enum class DataProcessingOpcode
{
    AND, EOR, SUB, RSB,
    ADD, ADC, SBC, RSC,
    TST, TEQ, CMP, CMN,
    ORR, MOV, BIC, MVN
};

using InstructionFunction = void (*)(uint32_t, GBA_CPU&); // Instruction Function Pointer alias

struct Instruction
{
    uint32_t rawInstruction;
    InstructionFunction function = nullptr;
    bool valid = false;
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

uint32_t ZeroExtendTo32(uint8_t value);
uint32_t ZeroExtendTo32(uint16_t value);

int32_t SignExtendTo32(uint8_t value);
int32_t SignExtendTo32(uint16_t value);

inline uint32_t CarryFrom(uint64_t result)
{
    return static_cast<uint32_t>(result >> 32);
}

uint32_t NumberOfSetBitsIn(uint32_t value);
