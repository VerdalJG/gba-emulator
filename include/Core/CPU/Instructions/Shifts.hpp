#pragma once
#include <cstdint>
#include "InstructionHelpers.hpp"

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

// LSR
uint32_t LogicalShiftRight(uint32_t value, uint32_t shiftImm);

// ASR
uint32_t ArithmeticShiftRight(uint32_t value, uint32_t shiftImm);

// ROR
uint32_t RotateRight(uint32_t value, uint32_t rotation);

//RRX
uint32_t RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu);


