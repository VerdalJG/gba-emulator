#pragma once
#include <cstdint>

namespace CPU_Timings
{
    constexpr uint32_t PIPELINE_FLUSH_PENALTY = 2;
    constexpr uint32_t MODE_SWITCH_PENALTY = 1;
    constexpr uint32_t OP2_SHIFTED_REGISTER_PENALTY = 1;
    constexpr uint32_t ALU_BASE_COST = 1;
}

uint32_t CalculateMultiplierCycles(uint32_t rsValue)
{
    if ((rsValue & 0xFFFFFF00) == 0x00000000 || (rsValue & 0xFFFFFF00) == 0xFFFFFF00)
    {
        return 1;
    } 

    if ((rsValue & 0xFFFF0000) == 0x00000000 || (rsValue & 0xFFFF0000) == 0xFFFF0000)
    {
        return 2;
    }

    if ((rsValue & 0xFF000000) == 0x00000000 || (rsValue & 0xFF000000) == 0xFF000000)
    {
        return 3;
    }

    return 4;
}
