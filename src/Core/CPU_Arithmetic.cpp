#include "GBA_CPU.hpp"
#include "InstructionHelpers.hpp"

void GBA_CPU::Add(uint32_t instruction) // ADD Rd, Rn Op2
{   
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction);

    uint32_t result = registers[values.rn] + values.op2;
    registers[values.rd] = result;

    // Set flags on CPSR
    if (values.setFlags) 
    {
        CPSRFlags flags = ProcessResultCPSRFlags(result, registers[values.rn], values.op2);
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
