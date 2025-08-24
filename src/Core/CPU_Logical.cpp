#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::LogicalAND(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t result = registers[values.rn] & values.op2.value;
    registers[values.rd] = result;

    if (!values.sFlag) return;

    if (values.rd == 15)
    {
        if (CurrentModeHasSPSR())
        {
            cpsr = spsr;
        }
        else
        {
            // UNPREDICTABLE
        }
    }
    else
    {
        UpdateCPSR_Logical(result, values.op2.carryOut);
    }
}

void GBA_CPU::LogicalExclusiveOR(uint32_t instruction)
{

}

void GBA_CPU::LogicalOR(uint32_t instruction)
{

}

void GBA_CPU::Move(uint32_t instruction)
{

}

void GBA_CPU::BitClear(uint32_t instruction)
{
    DataProcessingDecodedInstruction values = DataProcessing_Decode(instruction, *this);

    uint32_t result = registers[values.rn] & ~values.op2.value;
    registers[values.rd] = result;
    
    if (!values.sFlag) return;

    if (values.rd == 15)
    {
        if (CurrentModeHasSPSR())
        {
            cpsr = spsr;
        }
        else
        {
            // UNPREDICTABLE
        }
    }
    else
    {
        UpdateCPSR_Logical(result, values.op2.carryOut);
    }
}

void GBA_CPU::MoveNot(uint32_t instruction)
{

}


void GBA_CPU::Test(uint32_t instruction)
{
    
}

void GBA_CPU::TestEquivalence(uint32_t instruction)
{

}


