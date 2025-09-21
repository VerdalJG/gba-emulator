#pragma once
#include <cstdint>
#include <array>
#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"
#include "Core/CPU/CPU_CPSR.hpp"

using DataProcessingInstruction = void (*)(DataProcessing_Decoded, ShifterOperand, GBA_CPU&); // Data processing operation function pointers

static const int DATA_PROCESSING_OPCODE_COUNT = 16;

void DataProcessing(uint32_t instruction, GBA_CPU& cpu);

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
inline bool IsReverseOpcode(DataProcessingOpcode opcode)
{
    return opcode == DataProcessingOpcode::RSB || opcode == DataProcessingOpcode::RSC;
}
bool IsSubtractionOpcode(DataProcessingOpcode opcode);
bool ShouldUseCarryIn(DataProcessingOpcode opcode);

void UpdateCPSR_Arithmetic(DataProcessing_Decoded values, uint32_t rn, uint32_t op2, uint32_t result, GBA_CPU& cpu);
void UpdateCPSR_Logical(uint32_t result, uint32_t op2CarryOut, GBA_CPU& cpu);

// Templating is better performance wise and 
// avoids some errors on specific compilers vs 'auto' as parameter
template <typename Func>
void ArithmeticOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);
    uint32_t carryIn = ShouldUseCarryIn(values.opcode) ? cpu.GetCPSR_C() : 0;

    uint32_t result = operation(rn, op2.value, carryIn);
    cpu.SetValueAtRegister(values.rdIndex, result);

    // CPSR
    if (!values.setCPSRFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCPSRCase(cpu);
    }

    UpdateCPSR_Arithmetic(values, rn, op2.value, result, cpu);
}

template <typename Func>
void ArithmeticComparisonOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t result = operation(rn, op2.value);

    UpdateCPSR_Arithmetic(values, rn, op2.value, result, cpu);
}

template <typename Func>
void LogicalOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);

    uint32_t result = operation(rn, op2.value);
    cpu.SetValueAtRegister(values.rdIndex, result);

    if (!values.setCPSRFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCPSRCase(cpu);
    }

    UpdateCPSR_Logical(result, op2.carryOut, cpu);
}

template <typename Func>
void LogicalTestOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t result = operation(rn, op2.value);

    UpdateCPSR_Logical(result, op2.carryOut, cpu);
}

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