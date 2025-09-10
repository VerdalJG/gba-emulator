#include "Core/CPU_Shifts.hpp"
#include "Core/GBA_CPU.hpp"

Operand2Result LogicalLeft(uint32_t value, uint32_t shift, GBA_CPU& cpu)
{
    Operand2Result result;

    if (shift == 0)
    {
        result.carryOut = cpu.GetCPSR_C(); // Use old cpsr C flag if shift is 0
    }
    else if (shift < 32)
    {
        result.carryOut = (value >> (32 - shift)) & 1;
    }
    else // Special case when shift == 32 
    {
        result.carryOut = shift == 32 ? value & 1 : 0;
    }

    result.value = LogicalLeft(value, shift);

    return result;
}

uint32_t LogicalLeft(uint32_t value, uint32_t shiftImm)
{
    if (shiftImm < 32)
    {
        return value << shiftImm;
    }
    else // Cover for shiftImm >= 32 causing undefined behavior in C++
    {
        return 0;
    }
}

Operand2Result Op2_LogicalRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu)
{
    Operand2Result result;

    // Special case - Shift == 0 means LSR #32 - Only when op2 has immediate shifter
    if (shift == 0)
    {
        result.value = isImmediate ? 0 : value;
        result.carryOut = isImmediate ? value >> 31 : cpu.GetCPSR_C();
    }
    else if (shift < 32)
    {
        result.value = value >> shift;
        result.carryOut = (value >> (shift - 1)) & 1;
    }
    else 
    {
        result.value = 0;
        result.carryOut = shift == 32 ? (value >> 31) & 1 : 0;
    }
    
    return result; 
}

uint32_t LogicalRight(uint32_t value, uint32_t shiftImm)
{
    if (shiftImm == 0) // LSR #32
    {
        shiftImm = 32;
    }

    if (shiftImm < 32)
    {
        return value >> shiftImm;
    }
    else
    {
        return 0;
    }
    

    return 0;
}

Operand2Result Op2_ArithmeticRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu)
{
    Operand2Result result;

    if (shift == 0 && !isImmediate)
    {
        result.value = value;
        result.carryOut = cpu.GetCPSR_C();
        return result;
    }

    bool isNegative = (value & 0x80000000);
    result.value = ArithmeticRight(value, shift);

    if (shift >= 32)
    {
        result.carryOut = isNegative ? 1 : 0;
    }
    else
    {
        result.carryOut = (value >> (shift - 1)) & 1;
    }

    return result;
}

uint32_t ArithmeticRight(uint32_t value, uint32_t shiftImm)
{
    bool isNegative = (value & 0x80000000);
    if (shiftImm == 0) // ASR #32
    {
        shiftImm = 32;
    }

    if (shiftImm >= 32)
    {
        return isNegative ? 0xFFFFFFFF : 0;
    }

    if (isNegative)
    {
        uint32_t sign = 0xFFFFFFFF << (32 - shiftImm);
        return (value >> shiftImm) | sign;
    }
    else
    {
        return value >> shiftImm;
    }
}

Operand2Result Op2_RotateRight(uint32_t value, uint32_t rotation, bool isImmediate, GBA_CPU& cpu)
{
    Operand2Result result;

    if (rotation == 0)
    {
        if (isImmediate)
        {
            return RotateRightExtendCarry(value, cpu);
        }
        else 
        {
            result.value = value;
            result.carryOut = cpu.GetCPSR_C();
        }
    }

    rotation %= 32;
    result.value = (value >> rotation) | (value << (32 - rotation));
    result.carryOut = (value >> (rotation - 1)) & 1;
    return result;
}

uint32_t RotateRight(uint32_t value, uint32_t rotation)
{
    rotation %= 32; // Max rotation is 32 anyways
    return (value >> rotation) | (value << (32 - rotation));
}


Operand2Result RotateRightExtendCarry(uint32_t value, GBA_CPU &cpu)
{
    Operand2Result result;
    uint32_t cFlag = cpu.GetCPSR_C();

    result.value = (cFlag << 31) | (value >> 1);
    result.carryOut = value & 1;

    return result;
}

uint32_t CalculateScaledRegister(uint32_t rm, ShiftType shift, uint32_t shiftImm)
{
    switch(shift)
    {
        case ShiftType::LSL:
        return LogicalLeft(rm, shiftImm);

        case ShiftType::LSR:
        return LogicalRight(rm, shiftImm);

        case ShiftType::ASR:
        return ArithmeticRight(rm, shiftImm);

        case ShiftType::ROR:
        return RotateRight(rm, shiftImm);
    }
}
