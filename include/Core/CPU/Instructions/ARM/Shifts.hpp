#pragma once
#include "Utils/Integer.hpp"
#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"

class GBA_CPU;

enum ShiftType
{
    LSL,
    LSR,
    ASR,
    ROR
};

ShiftType GetShiftType_AddressingMode1(uint16_t shifterOperandBits);

// LSL
uint32_t LogicalShiftLeft(uint32_t value, uint32_t shiftAmount);

void LogicalShiftLeft2(u32& value, u32 shift, u32& carry);

// LSR
uint32_t LogicalShiftRight(uint32_t value, uint32_t shiftImm);

void LogicalShiftRight2(u32& value, u32 shift, u32& carry, bool immediate);

// ASR
uint32_t ArithmeticShiftRight(uint32_t value, uint32_t shiftImm);

void ArithmeticShiftRight2(u32& value, u32 shift, u32& carry, bool immediate);

// ROR
uint32_t RotateRight(uint32_t value, uint32_t rotation);

void RotateRight2(u32& value, u32 shift, u32& carry, bool immediate);

//RRX
uint32_t RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu);

void ApplyShift(int shiftOp, u32& value, u32 shift, u32& carry, bool immediate);


