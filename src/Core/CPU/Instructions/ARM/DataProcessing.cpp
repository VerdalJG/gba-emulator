#include "Core/CPU/Instructions/ARM/DataProcessing.hpp"
#include "Core/CPU/Instructions/ARM/AddressingMode1.hpp"
#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"
#include "Core/CPU/CPU_Timings.hpp"

void DataProcessing(uint32_t instruction, GBA_CPU &cpu)
{
    DataProcessing_Decoded values = DataProcessing_Decode(instruction);
    int functionId = static_cast<int>(values.opcode);
    DataProcessingInstruction alu = dataProcessingFuncTable[functionId];
    ShifterOperand op2 = CalculateOp2_AddressingMode1(values.shifterOperandBits, values.immediateFlag, cpu);

    // Execute operation
    alu(values, op2, cpu);
}

DataProcessing_Decoded DataProcessing_Decode(uint32_t instruction)
{
    DataProcessing_Decoded result;

    result.condition = GetConditionType(instruction);
    result.immediateFlag = (instruction >> 25) & 1;
    result.opcode = GetDataProcessingOpcode(instruction);
    result.setCPSRFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;

    result.shifterOperandBits = instruction & 0xFFF;
    return result;
}

bool IsReverseOpcode(ARM_ALUOp opcode)
{
    return opcode == ARM_ALUOp::RSB || opcode == ARM_ALUOp::RSC;
}


bool IsSubtractionOpcode(ARM_ALUOp opcode) 
{
    switch (opcode)
    {
        case ARM_ALUOp::SUB:
        case ARM_ALUOp::SBC: 
        case ARM_ALUOp::RSC: 
        case ARM_ALUOp::RSB:
        case ARM_ALUOp::CMP:
        return true;
        
        default:
        return false;      
    }
}

bool ShouldUseCarryIn(ARM_ALUOp opcode)
{
    switch (opcode)
    {
        case ARM_ALUOp::ADC:
        case ARM_ALUOp::RSC:
        case ARM_ALUOp::SBC:
        return true;

        default:
        return false;
    }
}

void UpdateCPSR_Arithmetic(DataProcessing_Decoded values, uint32_t rn, uint32_t op2, uint32_t result, GBA_CPU& cpu)
{
    uint32_t carryIn = ShouldUseCarryIn(values.opcode) ? cpu.GetCPSR_C() : 0;
    bool isSub = IsSubtractionOpcode(values.opcode);

    uint32_t N = CPSR_IsNegative(result);
    uint32_t Z = CPSR_IsZero(result);
    uint32_t C;

    if (isSub)
    {
        if (IsReverseOpcode(values.opcode))
        {
            C = CPSR_NOTCarryFrom(op2, rn, carryIn);
        }
        else
        {
            C = CPSR_NOTCarryFrom(rn, op2, carryIn);
        }
    }
    else
    {
        C = CPSR_CarryFrom(rn, op2, carryIn);
    }

    uint32_t V = CPSR_OverflowFrom(rn, op2, result, isSub);

    uint32_t flags = N | Z | C | V;
    cpu.UpdateCPSR(flags, 0xF0000000);
}

void UpdateCPSR_Logical(uint32_t result, ShifterOperand op2, GBA_CPU &cpu)
{
    uint32_t N = CPSR_IsNegative(result);
    uint32_t Z = CPSR_IsZero(result);
    uint32_t flags = N | Z;

    // C flag is only affected if op2 is shifted    
    if (op2.shifted)
    {
        uint32_t C = op2.carryOut << 29;
        flags |= C;
    }

    cpu.UpdateCPSR(flags, 0xE0000000);
}
