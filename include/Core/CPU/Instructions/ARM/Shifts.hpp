#pragma once
#include "Utils/Integer.hpp"
#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"

class GBA_CPU;

enum class ShiftType
{
    LSL,
    LSR,
    ASR,
    ROR
};

void LSL(u32& value, u32 shift, u32& carry);
void LSR(u32& value, u32 shift, u32& carry, bool immediate);
void ASR(u32& value, u32 shift, u32& carry, bool immediate);
void ROR(u32& value, u32 shift, u32& carry, bool immediate);

void ApplyShift(int shiftOp, u32& value, u32 shift, u32& carry, bool immediate);


