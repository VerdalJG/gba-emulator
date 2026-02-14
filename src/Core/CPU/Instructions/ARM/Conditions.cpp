#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/ARM/Conditions.hpp"

Condition GetConditionType(uint32_t instruction)
{
    return static_cast<Condition>(instruction >> 28);
}
