#include "Core/GBA_CPU.hpp"

void GBA_CPU::Multiply(uint32_t instruction)
{
    MultiplyDecoded_Instruction values = Multiply_Decode(instruction, *this);

    uint32_t rm = registers[values.rmIndex];
    uint32_t rs = registers[values.rsIndex];
    uint32_t rn = registers[values.rnIndex];
    uint32_t rd = registers[values.rdIndex];
    
    bool usingPC = values.rmIndex == 15 || values.rsIndex == 15 || values.rdIndex == 15 || values.rnIndex == 15; 
    if (usingPC)
    {
        // UNPREDICTABLE
        return;
    }

    if (values.rdIndex == values.rmIndex)
    {
        // UNPREDICTABLE
        return;
    }

    // MUL and MLA
    rd = values.accumulateFlag ? (rm * rs + rn) : (rm * rs);

    uint32_t cpsrFlags = 0;

    // N flag
    cpsrFlags |= (rd & 0x80000000);

    // Z flag
    cpsrFlags |= (rd == 0);

    // Update cpsr
    cpsr = (cpsr & 0x3FFFFFFF) | cpsrFlags; // Preserve C and V flag by masking
    
    // C flag is technically unpredictable after a MULS in versions below ARM5
    // MUL truncates to lower 32 bits, so the result is the same for signed and unsigned numbers

}

void GBA_CPU::MultiplyLong(uint32_t instruction)
{
    
}

void GBA_CPU::SingleDataSwap(uint32_t instruction)
{
    
}

void GBA_CPU::BranchAndExchange(uint32_t instruction)
{

}

void GBA_CPU::HalfwordDataTransferRegister(uint32_t instruction)
{

}

void GBA_CPU::HalfwordDataTransferImmediate(uint32_t instruction)
{

}