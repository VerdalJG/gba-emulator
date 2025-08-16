#pragma once
#include <cstdint>

enum ShiftType
{
    LSL,
    LSR,
    ASR,
    ROR
};

struct ShiftResult 
{
    uint32_t result;
    bool carry;
};

// LSL
ShiftResult LogicalLeft(uint32_t value, unsigned int shift);

// LSR
ShiftResult LogicalRight(uint32_t value, unsigned int shift);

// ASR
ShiftResult ArithmeticRight(uint32_t value, unsigned int shift, bool isImmediate);

// ROR
ShiftResult RotateRight(uint32_t value, unsigned int rotation);

//RRX
ShiftResult RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu);
