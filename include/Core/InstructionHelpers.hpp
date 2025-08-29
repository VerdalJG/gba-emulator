#pragma once
#include <cstdint>
#include <utility>

constexpr uint32_t COND_BITS = 0xF0000000; // Bits 31-28
constexpr uint32_t OPCODE_BITS = 0x1E00000; // Bits 24-21

constexpr int DATA_PROCESSING_SET_CPSR_FLAGS_SHIFT = 20;

constexpr uint32_t OPERAND2_MASK = 0xFFF;

constexpr uint32_t OPCODE_MASK = 0xF;
constexpr int OPCODE_SHIFT = 21;

constexpr uint32_t INSTRUCTION_TYPE_MASK = 0b11;
constexpr int INSTRUCTION_TYPE_SHIFT = 26;

constexpr int OPERAND2_MODE_SHIFT = 25;

constexpr uint32_t CONDITION_MASK = 0xF;
constexpr int CONDITION_SHIFT = 28;

class GBA_CPU;

struct Operand2Result 
{
    uint32_t value;
    uint32_t carryOut : 1; // Store only 1 bit of information
};

enum ShiftType
{
    LSL,
    LSR,
    ASR,
    ROR
};

struct DataProcessing_DecodedInstruction
{
    uint8_t rnIndex, rdIndex;
    Operand2Result op2;
    bool setCpsrFlag; // Whether or not to set CPSR Flags
};

struct MultiplyDecoded_Instruction
{
    uint8_t rdIndex, rnIndex, rsIndex, rmIndex;
    bool accumulateFlag, setCpsrFlag;
};

struct MultiplyLong_DecodedInstruction
{
    uint8_t rdHiIndex, rdLoIndex, rnIndex, rmIndex;
    bool unsignedFlag, accumulateFlag, setCpsrFlag;
};

struct SingleDataSwap_DecodedInstruction
{
    uint8_t rnIndex, rdIndex, rmIndex;
    bool bFlag;
};

struct HalfwordDataTransferRegister_DecodedInstruction
{
    uint8_t rnIndex, rdIndex, rmIndex;
    bool pFlag, unsignedFlag, wFlag, lFlag;
};

struct HalfwordDataTransferImmediate_DecodedInstruction
{
    uint8_t rnIndex, rdIndex, offset1, offset2;
    bool pFlag, unsignedFlag, wFlag, lFlag;
};


DataProcessing_DecodedInstruction DataProcessing_Decode(uint32_t instruction, GBA_CPU& cpu);
MultiplyDecoded_Instruction Multiply_Decode(uint32_t instruction, GBA_CPU& cpu);
MultiplyLong_DecodedInstruction MultiplyLong_Decode(uint32_t instruction, GBA_CPU& cpu);



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

enum class DataProcessingOpcode
{
    AND, EOR, SUB, RSB,
    ADD, ADC, SBC, RSC,
    TST, TEQ, CMP, CMN,
    ORR, MOV, BIC, MVN
};

enum InstructionPattern : uint8_t 
{
    PATTERN_00, // Data Processing (immediate/register) and Misc: Multiply, Swap, Halfword transfer, Branch (BX)
    PATTERN_01, // Single Data Transfer or Undefined (bit 4 decides)
    PATTERN_10, // Block Data Transfer (LDM/STM) or Branch
    PATTERN_11, // Coprocessor or Software Interrupt
};

/// @brief Extracts the second operand for data processing
/// @param instruction The current instruction being executed
/// @param isImmediateValue Immediate value flag (bit 25)
/// @return Operand2 (bits 11-0)
Operand2Result ExtractOperand2(uint32_t instruction, GBA_CPU& cpu);

DataProcessingOpcode GetDataProcessingOpcode(uint32_t instruction);

Operand2Result ShiftByRegister(uint16_t operand2, ShiftType shiftType, GBA_CPU& cpu);
Operand2Result ShiftByImmediate(uint16_t operand2, ShiftType shiftType,  GBA_CPU& cpu);

inline bool CheckBits(uint32_t instruction, uint32_t shift, uint32_t mask, uint32_t expected)
{
    return ((instruction >> shift) & mask) == expected;
}
