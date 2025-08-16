#include "CPU_Shifts.hpp"
#include "GBA_CPU.hpp"
#include "InstructionHelpers.hpp"

ShiftResult LogicalLeft(uint32_t value, unsigned int shift)
{
    ShiftResult 
    return value << shift;
}

ShiftResult LogicalRight(uint32_t value, unsigned int shift)
{
    return (shift == 0) ? 0 : value >> shift; // ARM doesn't allow shift by 0, reads it as shift by 32
}

ShiftResult ArithmeticRight(uint32_t value, unsigned int shift, bool isImmediate)
{
    if (shift == 0)
    {
        if (!isImmediate)
        {
            return value;
        }

        shift = 32;
    }

    bool isNegative = (value & 0x80000000);

    if (shift >= 32)
    {
        return isNegative ? 0xFFFFFFFF : 0;
    }

    if (isNegative)
    {
        uint32_t sign = 0xFFFFFFFF << (32 - shift);
        return (value >> shift) | sign;
    }
    else
    {
        return value >> shift;
    }
}

ShiftResult RotateRight(uint32_t value, unsigned int rotation)
{
    rotation &= 31; // Limit to 0-31
    if (rotation == 0) 
    {
        return value;
    }

    return (value >> rotation) | (value << (32 - rotation));
}

ShiftResult RotateRightExtendCarry(uint32_t value, GBA_CPU &cpu)
{
    uint8_t cFlag = (cpu.GetCPSR() >> 29) & 1;
    return (cFlag << 31) | (value >> 1);
}
