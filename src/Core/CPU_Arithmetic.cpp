#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::Add(uint32_t instruction) // ADD Rd, Rn Op2
{   
    auto operation = [] (uint32_t a, uint32_t b, uint32_t) { return a + b; };
    ArithmeticOperation(instruction, operation, false);
}

void GBA_CPU::AddWithCarry(uint32_t instruction)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t carryIn) { return a + b + carryIn; };
    ArithmeticOperation(instruction, operation, false, true);
}

void GBA_CPU::SubtractWithCarry(uint32_t instruction)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t carryIn) { return a - b - carryIn; };
    ArithmeticOperation(instruction, operation, true, true);
}

void GBA_CPU::ReverseSubtractWithCarry(uint32_t instruction)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t carryIn) { return b - a - carryIn; };
    ArithmeticOperation(instruction, operation, true, true);
}

void GBA_CPU::Subtract(uint32_t instruction)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t) { return a - b; };
    ArithmeticOperation(instruction, operation, true);
}

void GBA_CPU::ReverseSubtract(uint32_t instruction)
{
    auto operation = [] (uint32_t a, uint32_t b, uint32_t) { return b - a; };
    ArithmeticOperation(instruction, operation, true);
}

void GBA_CPU::Compare(uint32_t instruction)
{
    auto operation = [] (uint32_t a, uint32_t b) { return a - b; };
    ArithmeticComparisonOperation(instruction, operation, true);
}

void GBA_CPU::CompareNegative(uint32_t instruction)
{
    auto operation = [] (uint32_t a, uint32_t b) { return a + b; };
    ArithmeticComparisonOperation(instruction, operation, false);
}
