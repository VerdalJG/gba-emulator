#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::ArithmeticOperation(uint32_t instruction, auto operation, bool isSub, bool useCarry)
{   
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t& rd = registers[values.rdIndex];
    
    uint32_t op2Value = values.op2.value;
    uint32_t carryIn = useCarry ? GetCpsrC() : 0;

    rd = operation(rn, op2Value, carryIn);

    // CPSR
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
        return;
    }

    if (isSub)
    {
        UpdateCPSR_Sub(rd, rn, op2Value, !carryIn);
    }
    else
    {
        UpdateCPSR_Add(rd, rn, op2Value, carryIn);
    }
}

void GBA_CPU::ArithmeticComparisonOperation(uint32_t instruction, auto operation, bool isSub)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;

    uint32_t result = operation(rn, op2Value);

    if (isSub)
    {
        UpdateCPSR_Sub(result, rn, op2Value);
    }
    else
    {
        UpdateCPSR_Add(result, rn, op2Value);
    }

}

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
