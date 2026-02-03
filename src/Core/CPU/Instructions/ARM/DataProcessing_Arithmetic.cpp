#include "Core/CPU/Instructions/ARM/DataProcessing.hpp"

void Add(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu) // ADD Rd, Rn Op2
{   
    auto operation = [] (uint32_t a, uint32_t b, uint32_t) { return a + b; };
    ArithmeticOperation(values, op2, operation, cpu);
}

void AddWithCarry(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t carryIn) { return a + b + carryIn; };
    ArithmeticOperation(values, op2, operation, cpu);
}

void SubtractWithCarry(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t carryIn) { return a - b - carryIn; };
    ArithmeticOperation(values, op2, operation, cpu);
}

void ReverseSubtractWithCarry(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t carryIn) { return b - a - carryIn; };
    ArithmeticOperation(values, op2, operation, cpu);
}

void Subtract(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t) { return a - b; };
    ArithmeticOperation(values, op2, operation, cpu);
}

void ReverseSubtract(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t) { return b - a; };
    ArithmeticOperation(values, op2, operation, cpu);
}

void Compare(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [] (uint32_t a, uint32_t b) { return a - b; };
    ArithmeticComparisonOperation(values, op2, operation, cpu);
}

void CompareNegative(DataProcessing_Decoded values, ShifterOperand op2, GBA_CPU& cpu)
{
    auto operation = [] (uint32_t a, uint32_t b) { return a + b; };
    ArithmeticComparisonOperation(values, op2, operation, cpu);
}
