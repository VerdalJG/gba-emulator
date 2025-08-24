#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::Add(uint32_t instruction) // ADD Rd, Rn Op2
{   
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t result = registers[values.rn] + values.op2.value;
    registers[values.rd] = result;

    // CPSR
    if (!values.sFlag) return;

    if (values.rd == 15)
    {
        if (CurrentModeHasSPSR())
        {
            cpsr = spsr;
        }
        else
        {
            // TODO: UNPREDICATBLE if executed in user mode / system mode as those do not have SPSR
        }
    }
    else
    {
        UpdateCPSR_Add(result, values.rn, values.op2.value, GetCpsrC());
    }
    
}

void GBA_CPU::AddWithCarry(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t result = registers[values.rn] + values.op2.value + GetCpsrC();
    registers[values.rd] = result;

    // CPSR
    if (!values.sFlag) return;

    if (values.rd == 15)
    {
        if (CurrentModeHasSPSR())
        {
            cpsr = spsr;
        }
        else
        {
            // TODO: UNPREDICATBLE
        }
    }
    else
    {
        UpdateCPSR_Add(result, values.rn, values.op2.value, GetCpsrC());
    }
}

void GBA_CPU::SubtractWithCarry(uint32_t instruction)
{

}

void GBA_CPU::ReverseSubtractWithCarry(uint32_t instruction)
{

}

void GBA_CPU::Subtract(uint32_t instruction)
{

}

void GBA_CPU::ReverseSubtract(uint32_t instruction)
{

}

void GBA_CPU::Compare(uint32_t instruction)
{

}

void GBA_CPU::CompareNegative(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t result = registers[values.rn] + values.op2.value;

    UpdateCPSR_Arithmetic(result, registers[values.rn], values.op2.value, false);
}
