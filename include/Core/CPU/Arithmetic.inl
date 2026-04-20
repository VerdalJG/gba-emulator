#pragma once
#include "Core/CPU/Registers.hpp"

#include "Utils/Integers.hpp"

inline u32 GBA_CPU::ADD(u32 op1, u32 op2, bool set_flags)
{
    u32 result = op1 + op2;

    if (set_flags) // Update CPSR
    {
        UpdateNZFlags(result);
        UpdateCFlag_Add(op1, op2);
        UpdateVFlag(op1, op2, result, false);
    }

    return result;
}

inline u32 GBA_CPU::SUB(u32 op1, u32 op2, bool set_flags)
{
    u32 result = op1 - op2;

    if (set_flags) // Update CPSR
    {
        UpdateNZFlags(result);
        UpdateCFlag_Sub(op1, op2);
        UpdateVFlag(op1, op2, result, true);
    }

    return result;
}

inline u32 GBA_CPU::ADC(u32 op1, u32 op2, bool set_flags)
{
    uint carry = GetCPSR_C();
    u32 result = op1 + op2 + carry;

    if (set_flags)
    {
        UpdateNZFlags(result);
        UpdateCFlag_Add(op1, op2, carry);
        UpdateVFlag(op1, op2, result, false);   
    }

    return result;
}

inline u32 GBA_CPU::SBC(u32 op1, u32 op2, bool set_flags)
{
    uint carry = GetCPSR_C();
    uint borrow = (1 - carry);
    u32 result = op1 - op2 - borrow;

    if (set_flags)
    {
        UpdateNZFlags(result);
        UpdateCFlag_Sub(op1, op2, borrow);
        UpdateVFlag(op1, op2, result, true);   
    }

    return result;
}

inline void GBA_CPU::UpdateNZFlags(u32 result)
{
    cpuState.cpsr.fields.n = result >> 31;
    cpuState.cpsr.fields.z = (result == 0);
}

inline void GBA_CPU::UpdateCFlag_Add(u32 op1, u32 op2, u32 carryIn) 
{
    u64 fullResult = static_cast<u64>(op1) + static_cast<u64>(op2) + carryIn;
    cpuState.cpsr.fields.c = (fullResult >> 32) & 1;
}

inline void GBA_CPU::UpdateCFlag_Sub(u32 op1, u32 op2, u32 borrowIn) 
{
    // For subtraction, C = 1 means no borrow.
    // Compare op1 against op2 + borrow_in.
    u64 fullBorrow = (static_cast<u64>(op2) + borrowIn);
    cpuState.cpsr.fields.c = (op1 >= fullBorrow);
}

inline void GBA_CPU::UpdateVFlag(u32 op1, u32 op2, u32 result, bool isSub)
{
    if (isSub)
    {
        // SUB / SBC
        u32 operandsDifferentSign = op1 ^ op2;
        u32 resultDifferentSign = result ^ op1;
        cpuState.cpsr.fields.v = (operandsDifferentSign & resultDifferentSign) >> 31;
    }
    else
    {
        // ADD / ADC
        u32 operandsSameSign = ~(op1 ^ op2);
        u32 resultDifferentSign = result ^ op1;
        cpuState.cpsr.fields.v = (operandsSameSign & resultDifferentSign) >> 31;
    }
}