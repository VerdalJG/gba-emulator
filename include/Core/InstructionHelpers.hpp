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

struct DataProcessingDecodedInstruction
{
    uint8_t rn, rd;
    Operand2Result op2;
    bool sFlag; // Whether or not to set CPSR Flags
};

DataProcessingDecodedInstruction DataProcessing_Decode(uint32_t instruction, GBA_CPU& cpu);


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

/// @brief Extracts the first operand register (Rn) and the destination register (Rd)
/// @param instruction The current instruction being executed
/// @return A pair containing Rn and Rd (in that order)
std::pair<uint8_t, uint8_t> DataProcessing_ExtractRnRd(uint32_t instruction);

/// @brief Extracts the second operand for data processing
/// @param instruction The current instruction being executed
/// @param isImmediateValue Immediate value flag (bit 25)
/// @return Operand2 (bits 11-0)
Operand2Result ExtractOperand2(uint32_t instruction, GBA_CPU& cpu);

bool DataProcessing_ShouldSetFlags(uint32_t instruction);

bool Bit25Set(uint32_t instruction);

DataProcessingOpcode GetDataProcessingOpcode(uint32_t instruction);



Operand2Result ShiftByRegister(uint16_t operand2, ShiftType shiftType, GBA_CPU& cpu);
Operand2Result ShiftByImmediate(uint16_t operand2, ShiftType shiftType,  GBA_CPU& cpu);


