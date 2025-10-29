#include "Core/CPU/Instructions/InstructionHelpers.hpp"
#include "Core/CPU/Instructions/Shifts.hpp"
#include "Core/CPU/Instructions/Conditions.hpp"
#include "Core/GBA_CPU.hpp"

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

Multiply_Decoded Multiply_Decode(uint32_t instruction)
{
    Multiply_Decoded result;

    result.condition = GetConditionType(instruction);
    result.accumulateFlag = (instruction >> 21) & 1;
    result.setCPSRFlag = (instruction >> 20) & 1;

    result.rdIndex = (instruction >> 16) & 0xF;
    result.rsIndex = (instruction >> 8) & 0xF;
    result.rmIndex = instruction & 0xF;


    return result;
}

MultiplyLong_Decoded MultiplyLong_Decode(uint32_t instruction)
{
    MultiplyLong_Decoded result;

    result.condition = GetConditionType(instruction);
    result.signedFlag = (instruction >> 22) & 1;
    result.accumulateFlag = (instruction >> 21) & 1;
    result.setCPSRFlag = (instruction >> 20) & 1;

    result.rdHiIndex = (instruction >> 16) & 0xF;
    result.rdLoIndex = (instruction >> 12) & 0xF;
    result.rsIndex = (instruction >> 8) & 0xF;
    result.rmIndex = instruction & 0xF;


    return result;
}

SingleDataSwap_Decoded SingleDataSwap_Decode(uint32_t instruction)
{
    SingleDataSwap_Decoded result;

    result.condition = GetConditionType(instruction);
    result.bFlag = (instruction >> 24) & 1;
    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;
    result.rmIndex = instruction & 0xF;

    return result;
}

HalfwordDataTransfer_Decoded HalfwordDataTransfer_Decode(uint32_t instruction)
{
    HalfwordDataTransfer_Decoded result;

    result.condition = GetConditionType(instruction);
    result.pFlag = (instruction >> 24) & 1;
    result.uFlag = (instruction >> 23) & 1;
    result.wFlag = (instruction >> 21) & 1;
    result.lFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;

    result.sFlag = (instruction >> 6) & 1;
    result.hFlag = (instruction >> 5) & 1;

    result.offsetBits = instruction & 0xF0F;
    return result;
}

SingleDataTransfer_Decoded SingleDataTransfer_Decode(uint32_t instruction)
{
    SingleDataTransfer_Decoded result;

    result.condition = GetConditionType(instruction);
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

BlockDataTransfer_Decoded BlockDataTransfer_Decode(uint32_t instruction)
{
    BlockDataTransfer_Decoded result;

    result.condition = GetConditionType(instruction);

    result.pFlag = (instruction >> 24) & 1;
    result.uFlag = (instruction >> 23) & 1;
    result.sFlag = (instruction >> 22) & 1;
    result.wFlag = (instruction >> 21) & 1;
    result.lFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.registerList = instruction & 0xFFFF;
    return result;
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
    return static_cast<DataProcessingOpcode>((instruction >> 21) & 0xF);
}

uint32_t NumberOfSetBitsIn(uint32_t value)
{
    uint32_t result = 0;
    while (value != 0)
    {
        value &= (value - 1);
        result++;
    }

    return result;
}

