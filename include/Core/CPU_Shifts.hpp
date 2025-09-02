#pragma once
#include <cstdint>
#include "InstructionHelpers.hpp"

class GBA_CPU;

// LSL
Operand2Result LogicalLeft(uint32_t value, uint32_t shift, GBA_CPU& cpu);

// LSR
Operand2Result LogicalRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu);

// ASR
Operand2Result ArithmeticRight(uint32_t value, uint32_t shift, bool isImmediate, GBA_CPU& cpu);

// ROR
Operand2Result Operand2_RotateRight(uint32_t value, uint32_t rotation, bool isImmediate, GBA_CPU& cpu);
uint32_t RotateRight(uint32_t value, uint32_t rotation);

//RRX
Operand2Result RotateRightExtendCarry(uint32_t value, GBA_CPU& cpu);
