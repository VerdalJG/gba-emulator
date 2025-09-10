#include "Core/InstructionHelpers.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/CPU_Shifts.hpp"
#include <assert.h>
#include "InstructionHelpers.hpp"

DataProcessing_Decoded DataProcessing_Decode(uint32_t instruction, GBA_CPU& cpu)
{
    DataProcessing_Decoded result;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;
    result.op2 = ExtractOperand2(instruction, cpu);
    result.setCpsrFlag = (instruction >> 20) & 1;

    return result;
}

Multiply_Decoded Multiply_Decode(uint32_t instruction)
{
    Multiply_Decoded result;

    result.rdIndex = (instruction >> 16) & 0xF;
    result.rsIndex = (instruction >> 8) & 0xF;
    result.rmIndex = instruction & 0xF;
    result.accumulateFlag = (instruction >> 21) & 1;
    result.setCpsrFlag = (instruction >> 20) & 1;

    return result;
}

MultiplyLong_Decoded MultiplyLong_Decode(uint32_t instruction)
{
    MultiplyLong_Decoded result;

    result.rdHiIndex = (instruction >> 16) & 0xF;
    result.rdLoIndex = (instruction >> 12) & 0xF;
    result.rsIndex = (instruction >> 8) & 0xF;
    result.rmIndex = instruction & 0xF;
    result.signedFlag = (instruction >> 22) & 1;
    result.accumulateFlag = (instruction >> 21) & 1;
    result.setCpsrFlag = (instruction >> 20) & 1;

    return result;
}

SingleDataSwap_Decoded SingleDataSwap_Decode(uint32_t instruction)
{
    SingleDataSwap_Decoded result;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;
    result.rmIndex = instruction & 0xF;
    result.bFlag = (instruction >> 24) & 1;

    return result;
}

HalfwordDataTransfer_Decoded HalfwordDataTransfer_Decode(uint32_t instruction)
{
    HalfwordDataTransfer_Decoded result;

    result.pFlag = (instruction >> 24) & 1;
    result.uFlag = (instruction >> 23) & 1;
    result.wFlag = (instruction >> 21) & 1;
    result.lFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;

    result.sFlag = (instruction >> 6) & 1;
    result.hFlag = (instruction >> 5) & 1;

    return result;
}

SingleDataTransfer_Decoded SingleDataTransfer_Decode(uint32_t instruction, const GBA_CPU& cpu)
{
    SingleDataTransfer_Decoded result;

    result.iFlag = (instruction >> 25) & 1;
    result.pFlag = (instruction >> 24) & 1;
    result.uFlag = (instruction >> 23) & 1;
    result.bFlag = (instruction >> 22) & 1;
    result.wFlag = (instruction >> 21) & 1;
    result.lFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;
    result.offsetBits = instruction & 0xFFF;

    return result;
}


Operand2Result ExtractOperand2(uint32_t instruction, GBA_CPU &cpu)
{
    uint16_t operand2 = instruction & OPERAND2_MASK;
    bool isImmediate = (instruction >> 25) & 1; // Immediate flag lies in bit 25
    if (isImmediate)
    {
        // Get the immediate value and rotation amount
        uint32_t imm8 = operand2 & 0xFF; // Zero Extended 32-bit value
        uint32_t immRot = ((operand2 >> 8) & 0xF) * 2;

        // Perform Rotation
        return Operand2_RotateRight(imm8, immRot, isImmediate, cpu);
    }
    else // Shifted value
    {
        bool bit4 = (operand2 >> 4) & 1;
        ShiftType shiftType = static_cast<ShiftType>((operand2 >> 5) & 3);
        if (bit4)
        {
            return ShiftByRegister(operand2, shiftType, cpu);
        }
        else
        {
            return ShiftByImmediate(operand2, shiftType, cpu);
        }        
    }
}


Operand2Result ShiftByRegister(uint16_t operand2, ShiftType shiftType, GBA_CPU &cpu)
{
    uint8_t rm = operand2 & 0xF; // Bits 3:0 carry the base register
    uint32_t rmValue = cpu.GetValueAtRegister(rm);

    uint8_t rs = (operand2 >> 8) & 0xF;
    uint8_t rsValue = cpu.GetValueAtRegister(rs) & 0xFF; // Only bottom byte is used

    switch (shiftType)
    {
        case ShiftType::LSL:
        return LogicalLeft(rmValue, rsValue, cpu);

        case ShiftType::LSR:
        return LogicalRight(rmValue, rsValue, false, cpu);

        case ShiftType::ASR:
        return ArithmeticRight(rmValue, rsValue, false, cpu);
        
        case ShiftType::ROR:
        return Operand2_RotateRight(rmValue, rsValue, false, cpu);

        default:
        assert(false && "Invalid ShiftType"); // Unreachable case
        return {}; // return a default-constructed Operand2Result
    }
}

Operand2Result ShiftByImmediate(uint16_t operand2, ShiftType shiftType, GBA_CPU &cpu)
{
    uint8_t rm = operand2 & 0xF; // Bits 3:0 carry the base register
    uint32_t rmValue = cpu.GetValueAtRegister(rm);
    uint8_t shiftImm = (operand2 >> 7) & 0x1F;

    switch (shiftType)
    {
        case ShiftType::LSL:
        return LogicalLeft(rmValue, shiftImm, cpu);

        case ShiftType::LSR:
        return LogicalRight(rmValue, shiftImm, true, cpu);

        case ShiftType::ASR:
        return ArithmeticRight(rmValue, shiftImm, true, cpu);
        
        case ShiftType::ROR: // RRX handled inside ROR
        return Operand2_RotateRight(rmValue, shiftImm, true, cpu);

        default:
        assert(false && "Invalid ShiftType"); // Unreachable case
        return {}; // return a default-constructed Operand2Result
    }
}

uint32_t ZeroExtendTo32(uint8_t value)
{
    return static_cast<uint32_t>(value);
}

uint32_t ZeroExtendTo32(uint16_t value)
{
    return static_cast<uint32_t>(value);
}

int32_t SignExtendTo32(uint8_t value)
{
    return static_cast<int32_t>(static_cast<int8_t>(value));
}

int32_t SignExtendTo32(uint16_t value)
{
    return static_cast<int32_t>(static_cast<int16_t>(value));
}

DataProcessingOpcode GetDataProcessingOpcode(uint32_t instruction)
{
    return static_cast<DataProcessingOpcode>((instruction >> OPCODE_SHIFT) & OPCODE_MASK);
}
