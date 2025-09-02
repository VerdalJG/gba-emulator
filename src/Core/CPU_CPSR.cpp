#include "Core/GBA_CPU.hpp"

void GBA_CPU::UpdateCPSR_Add(uint32_t result, uint32_t op1, uint32_t op2, bool carryIn) 
{
    UpdateCPSR_Arithmetic(result, op1, op2, false, carryIn);
}

void GBA_CPU::UpdateCPSR_Sub(uint32_t result, uint32_t op1, uint32_t op2, bool carryIn)
{
    UpdateCPSR_Arithmetic(result, op1, op2, true, carryIn);
}

void GBA_CPU::UpdateCPSR_Arithmetic(uint32_t result, uint32_t op1, uint32_t op2, bool isSub, bool carryIn)
{
    // Negative check: Bit 31 is 1
    uint32_t N = result & 0x80000000;

    // Zero check: Result == 0
    uint32_t Z = ((result == 0) ? 1 : 0) << 30;

    // Carry check: UNSIGNED integer overflow
    uint32_t C;

    if (isSub)
    {
        // SUB / SBC: C = 1 if no borrow (op1 >= op2 + carryIn)
        uint64_t fullBorrow = (static_cast<uint64_t>(op2) + (carryIn ? 0 : 1));
        C = op1 >= fullBorrow ? (1 << 29) : 0;
    }
    else
    {
        // ADD / ADC: C = 1 if unsigned overflow
        uint64_t fullResult = static_cast<uint64_t>(op1) + static_cast<uint64_t>(op2) + (carryIn ? 1 : 0);
        C = (fullResult >> 32) << 29;
    }

    // Overflow check: SIGNED integer overflow
    uint32_t V;

    if (isSub)
    {
        // SUB / SBC
        bool operandsDifferentSign = ((op1 ^ op2) & 0x80000000) != 0;
        bool resultDifferentSign = ((result ^ op1) & 0x80000000) != 0;
        V = operandsDifferentSign && resultDifferentSign ? (1 << 28) : 0;
    }
    else
    {
        // ADD / ADC
        bool operandsSameSign = ((op1 ^ op2) & 0x80000000) == 0;
        bool resultDifferentSign = ((result ^ op1) & 0x80000000) != 0;
        V = operandsSameSign && resultDifferentSign ? (1 << 28) : 0;
    }

    // Combine flags
    uint32_t flags = N | Z | C | V;

    // Update cpsr
    cpsr = (cpsr & 0x0FFFFFFF) | flags;
}

void GBA_CPU::UpdateCPSR_Logical(uint32_t result, bool shifterCarryOut)
{
    uint32_t flags = 0;

    // Negative check: Bit 31 is 1
    if (result & 0x80000000)
    {
        flags |= (1 << 31);
    }

    // Zero check: Result == 0
    if (result == 0)
    {
        flags |= (1 << 30);
    }

    // Carry check: Use shifter carry out
    if (shifterCarryOut)
    {
        flags |= (1 << 29);
    }

    // Update cpsr
    cpsr = (cpsr & 0x1FFFFFFF) | flags; // Preserve V flag by masking
}

void GBA_CPU::UpdateCPSR(uint8_t flags, uint8_t flagsToUpdate)
{
}


void GBA_CPU::HandleProgramCounterCpsrCase()
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

uint32_t GBA_CPU::CarryFrom(uint64_t result)
{
    return static_cast<uint32_t>(result >> 32);
}