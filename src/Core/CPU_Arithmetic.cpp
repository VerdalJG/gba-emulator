#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::Add(uint32_t instruction) // ADD Rd, Rn Op2
{   
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t& rd = registers[values.rdIndex];
    rd = rn + op2Value;

    // CPSR
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Add(rd, rn, op2Value, GetCpsrC());
    }
    
}

void GBA_CPU::AddWithCarry(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t& rd = registers[values.rdIndex];
    rd = rn + op2Value + GetCpsrC();

    // CPSR
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Add(rd, rn, op2Value, GetCpsrC());
    }
}

void GBA_CPU::SubtractWithCarry(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t& rd = registers[values.rdIndex];
    uint32_t borrow = !GetCpsrC();
    rd = rn - op2Value - borrow;

    // CPSR
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Sub(rd, rn, op2Value, borrow);
    }
}

void GBA_CPU::ReverseSubtractWithCarry(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t& rd = registers[values.rdIndex];
    uint32_t borrow = !GetCpsrC();
    rd = op2Value - rn  - borrow;

    // CPSR
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Sub(rd,  op2Value, rn, borrow);
    }
}

void GBA_CPU::Subtract(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t& rd = registers[values.rdIndex];
    uint32_t borrow = !GetCpsrC();
    rd = rn - op2Value - borrow;

    // CPSR
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Sub(rd, rn, op2Value);
    }
}

void GBA_CPU::ReverseSubtract(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t& rd = registers[values.rdIndex];
    rd = op2Value - rn;

    // CPSR
    if (!values.sFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCpsrCase();
    }
    else
    {
        UpdateCPSR_Sub(rd, op2Value, rn);
    }
}

void GBA_CPU::Compare(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t result = rn - op2Value;

    UpdateCPSR_Sub(result, rn, op2Value);
}

void GBA_CPU::CompareNegative(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t op2Value = values.op2.value;
    uint32_t result = rn + op2Value;

    UpdateCPSR_Add(result, rn, op2Value);
}
