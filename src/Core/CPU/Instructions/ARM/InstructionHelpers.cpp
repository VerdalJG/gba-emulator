#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"
#include "Core/CPU/Instructions/ARM/Shifts.hpp"
#include "Core/CPU/Instructions/ARM/Conditions.hpp"
#include "Core/GBA_CPU.hpp"

DataProcessingOpcode GetDataProcessingOpcode(uint32_t instruction)
{
    return static_cast<DataProcessingOpcode>((instruction >> 21) & 0xF);
}



