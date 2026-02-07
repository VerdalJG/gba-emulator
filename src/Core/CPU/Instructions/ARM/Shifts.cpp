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

void LogicalShiftLeft2(u32& value, u32 shift, u32& carry) 
{
    // Calculate shift
    const u32 shiftAmount = std::min<u32>(shift, 33);
    const u32 result = static_cast<u32>((static_cast<u64>(value) << shiftAmount));
    value = result;

    // Calculate carry-out
    if (shiftAmount == 0) return;
    carry = static_cast<u32>((static_cast<u64>(value) << (shiftAmount - 1)) >> 31);
}


uint32_t LogicalShiftRight(uint32_t value, uint32_t shiftImm) 
{
    if (shiftImm == 0) // LSR #32
    {
        shiftImm = 32;
    }

    return shiftImm < 32 ? value >> shiftImm : 0;
}

void LogicalShiftRight2(u32& value, u32 shift, u32& carry, bool immediate) 
{   
    // LSR #32 is encoded as LSR #0
    if (immediate && shift == 0) 
    {
        shift = 32;
    }

    // Calculate shift
    const u32 shiftAmount = std::min<u32>(shift, 33);
    const u32 result = static_cast<u32>((static_cast<u64>(value) >> shiftAmount));
    value = result;

    // Calculate carry-out
    if (shiftAmount == 0) return;
    carry = static_cast<u32>((static_cast<u64>(value) >> (shiftAmount - 1)) & 1u);
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

void ArithmeticShiftRight2(u32& value, u32 shift, u32& carry, bool immediate) 
{
    // ASR #32 is encoded as ASR #0
    if (immediate && shift == 0) 
    {
        shift = 32;
    }

    // Calculate shift
    const u32 shiftAmount = std::min<u32>(shift, 33);
    const s64 signedValue = static_cast<s64>(static_cast<s32>(value));
    const u32 result = static_cast<u32>(signedValue >> shiftAmount);
    value = result;

    // Calculate carry-out
    if (shiftAmount == 0) return;
    carry = static_cast<u32>(signedValue >> (shiftAmount - 1) & 1u);
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

void RotateRight2(u32& value, u32 shift, u32& carry, bool immediate) 
{
    // RRX #1 is encoded as R0R #0
    if (immediate && shift == 0)
    {
        const u32 bit0 = value & 1;
        value = (value >> 1) | (carry << 31);
        carry = bit0;
    }

    if (shift == 0) return;

    const u32 shiftAmount = shift & 31;
    value = (value >> shiftAmount) | (value << ((32 - shiftAmount) & 31u));
    carry = value >> 31;
}

uint32_t RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu)
{
    uint32_t cFlag = cpu.GetCPSR_C();
    return LogicalShiftLeft(cFlag, 31) | LogicalShiftRight(value, 1);
}

void ApplyShift(int shiftOp, u32& value, u32 shift, u32& carry, bool immediate) 
{
    switch (static_cast<ShiftType>(shiftOp))
    {
        case LSL: LogicalShiftLeft2(value, shift, carry);
        case LSR: LogicalShiftRight2(value, shift, carry, immediate);
        case ASR: ArithmeticShiftRight2(value, shift, carry, immediate);
        case ROR: RotateRight2(value, shift, carry, immediate);
    }
}
