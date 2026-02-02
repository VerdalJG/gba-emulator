#pragma once
#include <cstdint>
#include <array>
#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/ARM/Conditions.hpp"

struct DataProcessing_Decoded
{
    Condition condition;
    uint8_t rnIndex, rdIndex;
    uint16_t shifterOperandBits;
    DataProcessingOpcode opcode;
    bool setCPSRFlag, immediateFlag;
};

using DataProcessingInstruction = void (*)(DataProcessing_Decoded, ShifterOperand, GBA_CPU&); // Data processing operation function pointers

static const int DATA_PROCESSING_OPCODE_COUNT = 16;

void DataProcessing(uint32_t instruction, GBA_CPU& cpu);
DataProcessing_Decoded DataProcessing_Decode(uint32_t instruction);

// In order of opcodes 0-15
void LogicalAND(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void LogicalExclusiveOR(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void Subtract(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void ReverseSubtract(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void Add(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void AddWithCarry(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void SubtractWithCarry(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void ReverseSubtractWithCarry(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void Test(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void TestEquivalence(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void Compare(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void CompareNegative(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void LogicalOR(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void Move(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void BitClear(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);
void MoveNot(DataProcessing_Decoded values, ShifterOperand shifterOperand, GBA_CPU& cpu);

// Is the opcode RSB/RSC?
bool IsReverseOpcode(DataProcessingOpcode opcode);
bool IsSubtractionOpcode(DataProcessingOpcode opcode);
bool ShouldUseCarryIn(DataProcessingOpcode opcode);

void UpdateCPSR_Arithmetic(DataProcessing_Decoded values, uint32_t rn, uint32_t op2, uint32_t result, GBA_CPU& cpu);
void UpdateCPSR_Logical(uint32_t result, ShifterOperand op2, GBA_CPU& cpu);

// Templating is better performance wise and 
// avoids some errors on specific compilers vs 'auto' as parameter
template <typename Func>
void ArithmeticOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu);

template <typename Func>
void ArithmeticComparisonOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu);

template <typename Func>
void LogicalOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu);

template <typename Func>
void LogicalTestOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu);

constexpr std::array<DataProcessingInstruction, 16> dataProcessingFuncTable {
    &LogicalAND,
    &LogicalExclusiveOR,
    &Subtract,
    &ReverseSubtract,
    &Add,
    &AddWithCarry,
    &SubtractWithCarry,
    &ReverseSubtractWithCarry,
    &Test,
    &TestEquivalence,
    &Compare,
    &CompareNegative,
    &LogicalOR,
    &Move,
    &BitClear,
    &MoveNot
};

#include "DataProcessing.tpp"