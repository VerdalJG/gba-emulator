#include "Core/CPU_Shifts.hpp"
#include "Core/GBA_CPU.hpp"

Operand2Result LogicalLeft(uint32_t value, uint32_t shift, GBA_CPU& cpu)
{
    Operand2Result result;

    if (shift < 32)
    {
        result.carryOut = (value >> (32 - shift)) & 1;
    }
    else if (shift == 32)
    {
        result.carryOut = value & 0x80000000; // Bit 31
    }
    else // Cover for shift == 0  and shift > 32
    {
        // Use old cpsr C flag if shift is 0
        result.carryOut = shift == 0 ? cpu.GetCPSR_C() : 0;
    }

    result.value = LogicalLeft(value, shift);
    return result;
}

uint32_t LogicalLeft(uint32_t value, uint32_t shiftImm)
{
    // Cover for shiftImm >= 32 causing undefined behavior in C++
    return shiftImm < 32 ? value << shiftImm : 0;
}

Operand2Result Op2_LogicalRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu)
{
    Operand2Result result;

    // Special case - Shift == 0 means LSR #32 - Only when op2 has immediate shifter
    if (shift == 0)
    {
        result.value = isImmediate ? 0 : value;
        result.carryOut = isImmediate ? value >> 31 : cpu.GetCPSR_C();
        return result;
    }
    else
    {
        result.carryOut = (value >> (shift - 1)) & 1;
    }

    result.value = LogicalRight(value, shift);
    
    return result; 
}

uint32_t LogicalRight(uint32_t value, uint32_t shiftImm)
{
    if (shiftImm == 0) // LSR #32
    {
        shiftImm = 32;
    }

    return shiftImm < 32 ? value >> shiftImm : 0;
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

    if (isImmediate)
    {
        if (rotation == 0)
        {
            return Op2_RotateRightExtendCarry(value, cpu);
        }
        
    }

    if (rotation == 0)
    {
        if (isImmediate)
        {
            return Op2_RotateRightExtendCarry(value, cpu);
        }
        else 
        {
            result.value = value;
            result.carryOut = cpu.GetCPSR_C();
            return result;
        }
    }

    rotation %= 32;
    result.value = (value >> rotation) | (value << (32 - rotation));
    result.carryOut = (value >> (rotation - 1)) & 1;
    return result;
}

uint32_t RotateRight(uint32_t value, uint32_t rotation, GBA_CPU& cpu)
{
    if (rotation == 0)
    {
        return RotateRightExtendCarry(value, cpu);
    }
    rotation %= 32; // Max rotation is 32 anyways
    return (value >> rotation) | (value << (32 - rotation));
}


Operand2Result Op2_RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu)
{
    Operand2Result result;
    result.value = RotateRightExtendCarry(value, cpu);
    result.carryOut = value & 1;

    return result;
}

uint32_t RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu)
{
    uint32_t cFlag = cpu.GetCPSR_C();
    return (cFlag << 31) | (value >> 1);
}
