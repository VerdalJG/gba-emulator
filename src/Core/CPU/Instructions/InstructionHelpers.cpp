#include "Core/CPU/Instructions/InstructionHelpers.hpp"
#include "Core/CPU/Instructions/Shifts.hpp"
#include "Core/CPU/Instructions/Conditions.hpp"
#include "Core/GBA_CPU.hpp"




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

