#include "Core/CPU/Instructions/ARM/Shifts.hpp"
#include "Core/GBA_CPU.hpp"

ShiftType GetShiftType_AddressingMode1(uint16_t shifterOperandBits)
{
    return static_cast<ShiftType>((shifterOperandBits >> 5) & 3);
}

uint32_t LogicalShiftLeft(uint32_t value, uint32_t shiftAmount)
{
    // Cover for shift >= 32 causing undefined behavior in C++
    return (shiftAmount < 32) ? (value << shiftAmount) : 0;
}

uint32_t LogicalShiftRight(uint32_t value, uint32_t shiftImm)
{
    if (shiftImm == 0) // LSR #32
    {
        shiftImm = 32;
    }

    return shiftImm < 32 ? value >> shiftImm : 0;
}

uint32_t ArithmeticShiftRight(uint32_t value, uint32_t shiftImm)
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

uint32_t RotateRight(uint32_t value, uint32_t rotation)
{
    if (rotation >= 32)
    {
        rotation %= 32; // Max rotation is 32 anyways
    }

    if (rotation == 0)
    {
        return value;
    }

    return (value >> rotation) | (value << (32 - rotation));
}

uint32_t RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu)
{
    uint32_t cFlag = cpu.GetCPSR_C();
    return LogicalShiftLeft(cFlag, 31) | LogicalShiftRight(value, 1);
}
