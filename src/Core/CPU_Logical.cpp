#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::LogicalOperation(uint32_t instruction, auto operation)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t& rd = registers[values.rdIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;
 
    rd = operation(rn, op2Value);

    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
        return;
    }

    UpdateCPSR_Logical(rd, op2CarryOut);
}

void GBA_CPU::LogicalTestOperation(uint32_t instruction, auto operation)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;

    uint32_t result = operation(rn, op2Value);

    UpdateCPSR_Logical(result, op2CarryOut);
}

void GBA_CPU::LogicalAND(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a & b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::LogicalExclusiveOR(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a ^ b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::LogicalOR(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a | b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::BitClear(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a & ~b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::Move(uint32_t instruction)
{
    auto operation = [](uint32_t, uint32_t b) { return b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::MoveNot(uint32_t instruction)
{
    auto operation = [](uint32_t, uint32_t b) { return ~b; };
    LogicalOperation(instruction, operation);
}


void GBA_CPU::Test(uint32_t instruction)
{
    auto operation = [](int a, int b) { return a & b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::TestEquivalence(uint32_t instruction)
{
    auto operation = [](int a, int b) { return a ^ b; };
    LogicalOperation(instruction, operation);
}


