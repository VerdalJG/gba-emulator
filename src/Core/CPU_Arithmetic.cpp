#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::Add(uint32_t instruction) // ADD Rd, Rn Op2
{   
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t result = registers[values.rn] + values.op2.value;
    registers[values.rd] = result;

    // Set flags on CPSR
    if (values.sFlag) 
    {
        CPSRFlags flags = ProcessResultCPSRFlags(result, registers[values.rn], values.op2.value);
        ApplyCPSRFlags(flags);
    }
}

void GBA_CPU::AddWithCarry(uint32_t instruction)
{

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

void GBA_CPU::CompareNegated(uint32_t instruction)
{
    
}
