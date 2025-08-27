#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::LogicalAND(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;
    uint32_t& rd = registers[values.rdIndex];

    rd = rn & op2Value;

    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Logical(rd, op2CarryOut);
    }
}

void GBA_CPU::LogicalExclusiveOR(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;
    uint32_t& rd = registers[values.rdIndex];

    rd = rn ^ op2Value;

    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Logical(rd, op2CarryOut);
    }
}

void GBA_CPU::LogicalOR(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;
    uint32_t& rd = registers[values.rdIndex];

    rd = rn | op2Value;

    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Logical(rd, op2CarryOut);
    }
}

void GBA_CPU::Move(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;
    uint32_t& rd = registers[values.rdIndex];

    rd = op2Value;

    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Logical(rd, op2CarryOut);
    }
}

void GBA_CPU::BitClear(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;
    uint32_t& rd = registers[values.rdIndex];

    rd = rn & ~op2Value;
    
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Logical(rd, op2CarryOut);
    }
}

void GBA_CPU::MoveNot(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;
    uint32_t& rd = registers[values.rdIndex];

    rd = ~op2Value;

    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Logical(rd, op2CarryOut);
    }
}


void GBA_CPU::Test(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;

    uint32_t result = rn & op2Value;

    UpdateCPSR_Logical(result, op2CarryOut);
}

void GBA_CPU::TestEquivalence(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    bool op2CarryOut = values.op2.carryOut;

    uint32_t result = rn ^ op2Value;

    UpdateCPSR_Logical(result, op2CarryOut);
}


