#include "InstructionHelpers.hpp"
#include "GBA_CPU.hpp"
#include "CPU_Shifts.hpp"

DataProcessingDecodedInstruction DataProcessing_Decode(uint32_t instruction)
{
    DataProcessingDecodedInstruction result;
    auto pair = DataProcessing_ExtractRnRd(instruction);
    bool immediate = Bit25Set(instruction);
    result.rn = pair.first;
    result.rd = pair.second;
    result.op2 = ExtractOperand2(instruction, immediate);
    result.setFlags = DataProcessing_ShouldSetFlags(instruction);

    return result;
}

std::pair<uint8_t, uint8_t> DataProcessing_ExtractRnRd(uint32_t instruction)
{
    uint8_t rn = (instruction >> 16) & 0xF;
    uint8_t rd = (instruction >> 12) & 0xF;
    return std::make_pair(rn, rd);
}

uint32_t ExtractOperand2(uint32_t instruction, bool IsImmediateValue, GBA_CPU& cpu)
{
    uint16_t operand2 = instruction & OPERAND2_MASK;
    if (IsImmediateValue)
    {
        // Get the immediate value and rotation amount
        uint32_t imm8 = operand2 & 0xFF; // Zero Extended 32-bit value
        uint32_t immRot = ((operand2 >> 8) & 0xF) * 2;

        // Perform Rotation
        return RotateRight(imm8, immRot);
    }
    else // Register shift value
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

CPSRFlags ProcessResultCPSRFlags(uint32_t result, uint32_t op1, uint32_t op2)
{
    CPSRFlags flags;

    // Negative check: Bit 31 is 1
    flags.N = (result >> 31) & 1;

    // Zero check: Result == 0
    flags.Z = result == 0;

    // Carry check: UNSIGNED integer overflow
    flags.C = (static_cast<uint64_t>(op1) + static_cast<uint64_t>(op2)) >> 32;

    // Overflow check: SIGNED integer overflow
    bool operandsSameSign = ((op1 ^ op2) & 0x80000000) == 0;
    bool resultDifferentSign = ((result ^ op1) & 0x80000000) != 0;
    flags.V = operandsSameSign && resultDifferentSign;

    return flags;
}

uint32_t ShiftByRegister(uint16_t operand2, ShiftType shiftType, GBA_CPU &cpu)
{
    uint8_t rm = operand2 & 0xF; // Bits 3:0 carry the base register
    uint32_t rmValue = cpu.GetValueAtRegister(rm);

    uint8_t rs = (operand2 >> 8) & 0xF;
    uint8_t rsValue = cpu.GetValueAtRegister(rs) & 0xFF; // Only bottom byte is used

    switch (shiftType)
    {
        case ShiftType::LSL:
        return LogicalLeft(rmValue, rsValue);

        case ShiftType::LSR:
        return LogicalRight(rmValue, rsValue);

        case ShiftType::ASR:
        return ArithmeticRight(rmValue, rsValue, false);
        
        case ShiftType::ROR:
        if (rsValue == 0)
        {
            // Special case: RRX
            return RotateRightExtendCarry(rmValue, cpu);
        }
        
        return RotateRight(rmValue, rsValue);
    }
}

uint32_t ShiftByImmediate(uint16_t operand2, ShiftType shiftType, GBA_CPU &cpu)
{
    uint8_t rm = operand2 & 0xF; // Bits 3:0 carry the base register
    uint32_t rmValue = cpu.GetValueAtRegister(rm);
    uint8_t shiftImm = (operand2 >> 7) & 0x1F;

    switch (shiftType)
    {
        case ShiftType::LSL:
        return LogicalLeft(rmValue, shiftImm);

        case ShiftType::LSR:
        return LogicalRight(rmValue, shiftImm);

        case ShiftType::ASR:
        return ArithmeticRight(rmValue, shiftImm, true);
        
        case ShiftType::ROR:
        if (shiftImm == 0)
        {
            // Special case: RRX
            return RotateRightExtendCarry(rmValue, cpu);
        }
        else
        {
            return RotateRight(rmValue, shiftImm);
        }
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
