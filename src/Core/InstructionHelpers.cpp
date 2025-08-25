#include "Core/InstructionHelpers.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/CPU_Shifts.hpp"
#include <assert.h>

DataProcessingDecodedInstruction DataProcessing_Decode(uint32_t instruction, GBA_CPU& cpu)
{
    DataProcessingDecodedInstruction result;
    auto pair = DataProcessing_ExtractRnRd(instruction);

    result.rn = pair.first;
    result.rd = pair.second;
    result.op2 = ExtractOperand2(instruction, cpu);
    result.sFlag = DataProcessing_ShouldSetFlags(instruction);

    return result;
}

std::pair<uint8_t, uint8_t> DataProcessing_ExtractRnRd(uint32_t instruction)
{
    uint8_t rn = (instruction >> 16) & 0xF;
    uint8_t rd = (instruction >> 12) & 0xF;
    return std::make_pair(rn, rd);
}

Operand2Result ExtractOperand2(uint32_t instruction, GBA_CPU& cpu)
{
    uint16_t operand2 = instruction & OPERAND2_MASK;
    bool isImmediate = Bit25Set(instruction);
    if (isImmediate)
    {
        // Get the immediate value and rotation amount
        uint32_t imm8 = operand2 & 0xFF; // Zero Extended 32-bit value
        uint32_t immRot = ((operand2 >> 8) & 0xF) * 2;

        // Perform Rotation
        return RotateRight(imm8, immRot, isImmediate, cpu);
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

bool DataProcessing_ShouldSetFlags(uint32_t instruction)
{
    return (instruction >> DATA_PROCESSING_SET_CPSR_FLAGS_SHIFT) & 1;
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
        return RotateRight(rmValue, rsValue, false, cpu);

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
        return RotateRight(rmValue, shiftImm, true, cpu);

        default:
        assert(false && "Invalid ShiftType"); // Unreachable case
        return {}; // return a default-constructed Operand2Result
    }
}

DataProcessingOpcode GetDataProcessingOpcode(uint32_t instruction)
{
    return static_cast<DataProcessingOpcode>((instruction >> OPCODE_SHIFT) & OPCODE_MASK);
}

bool Bit25Set(uint32_t instruction)
{
    return (instruction >> 25) & 1;
}
