#include "Core/CPU/Shifts.hpp"
#include "Core/GBA_CPU.hpp"

void LSL(u32& value, u32 shift, u32& carry) 
{
    // Calculate shift
    const u32 shiftAmount = std::min<u32>(shift, 33);
    const u32 result = static_cast<u32>((static_cast<u64>(value) << shiftAmount));
    value = result;

    // Calculate carry-out
    if (shiftAmount == 0) return;
    carry = static_cast<u32>((static_cast<u64>(value) << (shiftAmount - 1)) >> 31);
}

void LSR(u32& value, u32 shift, u32& carry, bool immediate) 
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

void ASR(u32& value, u32 shift, u32& carry, bool immediate) 
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

void ROR(u32& value, u32 shift, u32& carry, bool immediate) 
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

void ApplyShift(int shiftOp, u32& value, u32 shift, u32& carry, bool immediate) 
{
    switch (static_cast<ShiftType>(shiftOp))
    {
        case ShiftType::LSL: return LSL(value, shift, carry);
        case ShiftType::LSR: return LSR(value, shift, carry, immediate);
        case ShiftType::ASR: return ASR(value, shift, carry, immediate);
        case ShiftType::ROR: return ROR(value, shift, carry, immediate);
    }
}
