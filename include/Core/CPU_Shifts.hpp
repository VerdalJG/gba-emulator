#pragma once
#include <cstdint>
#include "InstructionHelpers.hpp"

class GBA_CPU;

// LSL
Operand2Result LogicalLeft(uint32_t value, uint32_t shift, GBA_CPU& cpu);
uint32_t LogicalLeft(uint32_t value, uint32_t shiftImm);

// LSR
Operand2Result Op2_LogicalRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu);
uint32_t LogicalRight(uint32_t value, uint32_t shiftImm);

// ASR
Operand2Result Op2_ArithmeticRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu);
uint32_t ArithmeticRight(uint32_t value, uint32_t shiftImm);

// ROR
Operand2Result Op2_RotateRight(uint32_t value, uint32_t rotation, bool isImmediate, GBA_CPU& cpu);
uint32_t RotateRight(uint32_t value, uint32_t rotation, GBA_CPU& cpu);

//RRX
Operand2Result Op2_RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu);
uint32_t RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu);


