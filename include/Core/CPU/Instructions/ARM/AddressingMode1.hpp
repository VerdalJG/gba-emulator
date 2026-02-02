#pragma once
#include "Core/CPU/Instructions/ARM/Shifts.hpp" 

class GBA_CPU;

ShifterOperand CalculateOp2_AddressingMode1(uint16_t shifterOperandBits, bool isImmediate, GBA_CPU& cpu);

ShifterOperand CalculateOp2_Immediate(uint16_t shifterOperandBits, GBA_CPU& cpu);
ShifterOperand CalculateOp2_Register(uint16_t shifterOperandBits, GBA_CPU& cpu);

ShifterOperand ShiftOp2_Immediate(uint16_t shifterOperandBits, ShiftType shiftType, GBA_CPU& cpu);
ShifterOperand ShiftOp2_Register(uint16_t shifterOperandBits, ShiftType shiftType, GBA_CPU& cpu);

ShifterOperand ShiftOp2_LSL_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU& cpu);
ShifterOperand ShiftOp2_LSL_Register(uint32_t rm, uint32_t rs, GBA_CPU& cpu);

ShifterOperand ShiftOp2_LSR_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU& cpu);
ShifterOperand ShiftOp2_LSR_Register(uint32_t rm, uint32_t rs, GBA_CPU& cpu);

ShifterOperand ShiftOp2_ASR_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU& cpu);
ShifterOperand ShiftOp2_ASR_Register(uint32_t rm, uint32_t rs, GBA_CPU& cpu);

ShifterOperand ShiftOp2_ROR_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU& cpu);
ShifterOperand ShiftOp2_ROR_Register(uint32_t rm, uint32_t rs, GBA_CPU& cpu);
ShifterOperand ShiftOp2_RRX(uint32_t rm, GBA_CPU &cpu);

