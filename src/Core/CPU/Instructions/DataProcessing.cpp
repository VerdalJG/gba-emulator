#include "Core/CPU/Instructions/DataProcessing.hpp"
#include "Core/CPU/Instructions/AddressingMode1.hpp"

void DataProcessing(uint32_t instruction, GBA_CPU &cpu)
{
    DataProcessing_Decoded values = DataProcessing_Decode(instruction);
    int functionId = static_cast<int>(values.opcode);
    DataProcessingInstruction alu = dataProcessingFuncTable[functionId];
    ShifterOperand op2 = CalculateOp2_AddressingMode1(values.shifterOperandBits, values.immediateFlag, cpu);

    // Execute operation
    alu(values, op2, cpu);
}
bool IsReverseOpcode(DataProcessingOpcode opcode)
{
    return opcode == DataProcessingOpcode::RSB || opcode == DataProcessingOpcode::RSC;
}


bool IsSubtractionOpcode(DataProcessingOpcode opcode) 
{
    switch (opcode)
    {
        case DataProcessingOpcode::SUB:
        case DataProcessingOpcode::SBC: 
        case DataProcessingOpcode::RSC: 
        case DataProcessingOpcode::RSB:
        case DataProcessingOpcode::CMP:
        return true;
        
        default:
        return false;      
    }
}

bool ShouldUseCarryIn(DataProcessingOpcode opcode)
{
    switch (opcode)
    {
        case DataProcessingOpcode::ADC:
        case DataProcessingOpcode::RSC:
        case DataProcessingOpcode::SBC:
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

void UpdateCPSR_Logical(uint32_t result, uint32_t op2CarryOut, GBA_CPU &cpu)
{
    uint32_t N = CPSR_IsNegative(result);
    uint32_t Z = CPSR_IsZero(result);
    uint32_t C = op2CarryOut << 29;

    uint32_t flags = N | Z | C;

    cpu.UpdateCPSR(flags, 0xE0000000);
}
