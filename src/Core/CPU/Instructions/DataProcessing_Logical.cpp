#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/DataProcessing.hpp"

void LogicalAND(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](uint32_t a, uint32_t b) { return a & b; };
    LogicalOperation(values, op2, operation, cpu);
}

void LogicalExclusiveOR(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](uint32_t a, uint32_t b) { return a ^ b; };
    LogicalOperation(values, op2, operation, cpu);
}

void LogicalOR(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](uint32_t a, uint32_t b) { return a | b; };
    LogicalOperation(values, op2, operation, cpu);
}

void BitClear(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](uint32_t a, uint32_t b) { return a & ~b; };
    LogicalOperation(values, op2, operation, cpu);
}

void Move(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](uint32_t, uint32_t b) { return b; };
    LogicalOperation(values, op2, operation, cpu);
}

void MoveNot(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](uint32_t, uint32_t b) { return ~b; };
    LogicalOperation(values, op2, operation, cpu);
}

void Test(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](int a, int b) { return a & b; };
    LogicalTestOperation(values, op2, operation, cpu);
}

void TestEquivalence(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [](int a, int b) { return a ^ b; };
    LogicalTestOperation(values, op2, operation, cpu);
}


