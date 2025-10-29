#include "Core/CPU/Instructions/AddressingMode1.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/Shifts.hpp"
#include <assert.h>

ShifterOperand CalculateOp2_AddressingMode1(uint16_t shifterOperandBits, bool isImmediate, GBA_CPU &cpu)
{
    if (isImmediate)
    {
        return CalculateOp2_Immediate(shifterOperandBits, cpu);
    }
    else
    {
        return CalculateOp2_Register(shifterOperandBits, cpu);
    }
}



ShifterOperand CalculateOp2_Immediate(uint16_t shifterOperandBits, GBA_CPU &cpu)
{
    ShifterOperand result;
    uint32_t immediate_8 = shifterOperandBits & 0xFF;
    uint32_t rotation = (shifterOperandBits >> 8) & 0xF;

    // The rotation amount is in steps of two, this also means not all 32-bit values are available, 
    // but allows more to be obtained
    result.value = RotateRight(immediate_8, rotation * 2);
    result.carryOut = rotation == 0 ? cpu.GetCPSR_C() : (result.value >> 31);

    return result;
}

ShifterOperand CalculateOp2_Register(uint16_t shifterOperandBits, GBA_CPU &cpu)
{
    ShiftType shiftOperation = GetShiftType_AddressingMode1(shifterOperandBits);
    bool bit4 = (shifterOperandBits >> 4) & 1;
    if (bit4) // Shift by register
    {
        return ShiftOp2_Register(shifterOperandBits, shiftOperation, cpu);
    }
    else // Shift by immediate
    {
        return ShiftOp2_Immediate(shifterOperandBits, shiftOperation, cpu);
    }
}

ShifterOperand ShiftOp2_Immediate(uint16_t shifterOperandBits, ShiftType shiftType, GBA_CPU &cpu)
{
    uint8_t rmIndex = shifterOperandBits & 0xF; // Bits 3:0 carry the base register index
    uint32_t rm = cpu.GetValueAtRegister(rmIndex);
    uint32_t shiftImm = (shifterOperandBits >> 7) & 0x1F; // Bits 11:7 

    switch (shiftType)
    {
        case ShiftType::LSL:
        return ShiftOp2_LSL_Immediate(rm, shiftImm, cpu);

        case ShiftType::LSR:
        return ShiftOp2_LSR_Immediate(rm, shiftImm, cpu);

        case ShiftType::ASR:
        return ShiftOp2_ASR_Immediate(rm, shiftImm, cpu);
        
        case ShiftType::ROR: // RRX handled inside ROR
        return ShiftOp2_ROR_Immediate(rm, shiftImm, cpu);

        default:
        assert(false && "Invalid ShiftType"); // Unreachable case
        return {}; // return a default-constructed Operand2Result
    }
}

ShifterOperand ShiftOp2_Register(uint16_t shifterOperandBits, ShiftType shiftType, GBA_CPU &cpu)
{
    uint8_t rmIndex = shifterOperandBits & 0xF; // Bits 3:0 carry the base register index
    uint32_t rm = cpu.GetValueAtRegister(rmIndex);

    uint8_t rsIndex = (shifterOperandBits >> 8) & 0xF;
    uint8_t rs = cpu.GetValueAtRegister(rsIndex) & 0xFF; // Only bottom byte is used for shifting

    // GBA QUIRK - Instead of UNPREDICTABLE here, we add +12 to current instruction,
    // which means +4 due to pipeline offset applying +8 already
    if (rmIndex == 15) rm += 4;

    switch (shiftType)
    {
        case ShiftType::LSL:
        return ShiftOp2_LSL_Register(rm, rs, cpu);

        case ShiftType::LSR:
        return ShiftOp2_LSR_Register(rm, rs, cpu);

        case ShiftType::ASR:
        return ShiftOp2_ASR_Register(rm, rs, cpu);
        
        case ShiftType::ROR: // RRX handled inside ROR
        return ShiftOp2_ROR_Register(rm, rs, cpu);

        default:
        assert(false && "Invalid ShiftType"); // Unreachable case
        return {}; // return a default-constructed Operand2Result
    }
}

ShifterOperand ShiftOp2_LSL_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU &cpu)
{
    ShifterOperand result;

    if (shiftImm == 0)
    {
        result.value = rm;
        result.carryOut = cpu.GetCPSR_C();
    }
    else
    {
        result.value = LogicalShiftLeft(rm, shiftImm);
        result.carryOut = rm >> (32 - shiftImm);
    }

    return result;
}

ShifterOperand ShiftOp2_LSL_Register(uint32_t rm, uint32_t rs, GBA_CPU &cpu)
{
    ShifterOperand result;

    if (rs == 0)
    {
        result.value = rm;
        result.carryOut = cpu.GetCPSR_C();
    }
    else if (rs < 32)
    {
        result.value = LogicalShiftLeft(rm, rs);
        result.carryOut = rm >> (32 - rs);
    }
    else if (rs == 32)
    {
        result.value = 0;
        result.carryOut = rm & 1;
    }
    else // rs > 32
    {
        result.value = 0;
        result.carryOut = 0;
    }

    return result;
}

ShifterOperand ShiftOp2_LSR_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU &cpu)
{
    ShifterOperand result;

    if (shiftImm == 0) // LSR #32
    {
        result.value = 0;
        result.carryOut = (rm >> 31);
    }
    else // shiftImm > 0
    {
        result.value = LogicalShiftRight(rm, shiftImm);
        result.carryOut = rm >> (shiftImm - 1);
    }

    return result;
}

ShifterOperand ShiftOp2_LSR_Register(uint32_t rm, uint32_t rs, GBA_CPU &cpu)
{
    ShifterOperand result;

    if (rs == 0)
    {
        result.value = rm;
        result.carryOut = cpu.GetCPSR_C();
    }
    else if (rs < 32)
    {
        result.value = LogicalShiftRight(rm, rs);
        result.carryOut = rm >> (rs - 1);
    }
    else if (rs == 32)
    {
        result.value = 0;
        result.carryOut = (rm >> 31);
    }
    else // rs > 32
    {
        result.value = 0;
        result.carryOut = 0;
    }

    return result;
}

ShifterOperand ShiftOp2_ASR_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU &cpu)
{
    ShifterOperand result;

    if (shiftImm == 0) // ASR #32
    {
        bool isNegative = rm & 0x80000000; // Bit 31

        if (isNegative)
        {
            result.value = 0xFFFFFFFF;
        }
        else
        {
            result.value = 0;
        }

        result.carryOut = rm >> 31;
    }
    else // shiftImm > 0
    {
        result.value = ArithmeticShiftRight(rm, shiftImm);
        result.carryOut = rm >> (shiftImm - 1);
    }

    return result;
}

ShifterOperand ShiftOp2_ASR_Register(uint32_t rm, uint32_t rs, GBA_CPU &cpu)
{
    ShifterOperand result;

    if (rs == 0)
    {
        result.value = rm;
        result.carryOut = cpu.GetCPSR_C();
    }
    else if (rs < 32)
    {
        result.value = ArithmeticShiftRight(rm, rs);
        result.carryOut = rm >> (rs - 1);
    }
    else if (rs >= 32)
    {
        bool isNegative = rm & 0x80000000; // Bit 31

        if (isNegative)
        {
            result.value = 0xFFFFFFFF;
        }
        else
        {
            result.value = 0;
        }

        result.carryOut = rm >> 31;
    }

    return result;
}

ShifterOperand ShiftOp2_ROR_Immediate(uint32_t rm, uint32_t shiftImm, GBA_CPU &cpu)
{
    ShifterOperand result;

    if (shiftImm == 0) // ROR #0 becomes RRX #1
    {
        return ShiftOp2_RRX(rm, cpu);
    }
    else // shiftImm > 0
    {
        result.value = RotateRight(rm, shiftImm);
        result.carryOut = rm >> (shiftImm - 1);
    }

    return result;
}

ShifterOperand ShiftOp2_ROR_Register(uint32_t rm, uint32_t rs, GBA_CPU &cpu)
{
    ShifterOperand result;

    // ROR by register uses the bits Rs[4:0] to limit rotation to 0-31, more efficient than mod 32
    uint32_t relevantBits_5 = (rs & 0x1F);

    if (rs == 0)
    {
        result.value = rm;
        result.carryOut = cpu.GetCPSR_C();
    }
    else if (relevantBits_5 == 0) // Rotating by a multiple of 32
    {
        result.value = rm;
        result.carryOut = (rm >> 31);
    }
    else // relevantBits_5 > 0
    {
        result.value = RotateRight(rm, relevantBits_5);
        result.carryOut = rm >> (relevantBits_5 - 1);
    }

    return result;
}

ShifterOperand ShiftOp2_RRX(uint32_t rm, GBA_CPU &cpu)
{
    ShifterOperand result;

    result.value = RotateRightExtendCarry(rm, cpu);
    result.carryOut = rm & 1; 

    return result;
}


// The instruction "MOV R0,R0" is used as "NOP" opcode in 32bit ARM state.
// Execution Time: (1+p)S+rI+pN. Whereas r=1 if I=0 and R=1 
// (ie. shift by register); otherwise r=0. And p=1 if Rd=R15; otherwise p=0.