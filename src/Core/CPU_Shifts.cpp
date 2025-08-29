#include "Core/CPU_Shifts.hpp"
#include "Core/GBA_CPU.hpp"

Operand2Result LogicalLeft(uint32_t value, uint32_t shift, GBA_CPU& cpu)
{
    Operand2Result result;

    if (shift == 0)
    {
        result.value = value;
        result.carryOut = cpu.GetCPSR_C(); // Use old cpsr C flag if shift is 0
    }
    else if (shift < 32)
    {
        result.value = value << shift;
        result.carryOut = (value >> (32 - shift)) & 1;
    }
    else // Must cover for shift >= 32 due to C++ undefined behavior when shifting over 32 bits
    {
        result.value = 0;
        result.carryOut = shift == 32 ? value & 1 : 0;
    }

    return result;
}

Operand2Result LogicalRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu)
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

Operand2Result ArithmeticRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu)
{
    Operand2Result result;

    if (shift == 0)
    {
        if (!isImmediate)
        {
            result.value = value;
            result.carryOut = cpu.GetCPSR_C();
            return result;
        }

        shift = 32;  // Special case - ASR #32 when op2 has immediate shifter
    }

    bool isNegative = (value & 0x80000000);

    if (shift >= 32)
    {
        result.value = isNegative ? 0xFFFFFFFF : 0;
        result.carryOut = isNegative ? 1 : 0;
        return result;
    }

    if (isNegative)
    {
        uint32_t sign = 0xFFFFFFFF << (32 - shift);
        result.value = (value >> shift) | sign;
    }
    else
    {
        result.value = value >> shift;
    }

    result.carryOut = (value >> (shift - 1)) & 1;
    return result;
}

Operand2Result RotateRight(uint32_t value, uint32_t rotation, bool isImmediate, GBA_CPU& cpu)
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

Operand2Result RotateRightExtendCarry(uint32_t value, GBA_CPU &cpu)
{
    Operand2Result result;
    uint32_t cFlag = cpu.GetCPSR_C();

    result.value = (cFlag << 31) | (value >> 1);
    result.carryOut = value & 1;

    return result;
}
