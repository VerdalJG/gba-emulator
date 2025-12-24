#pragma once
#include <cstdint>

namespace CPU_Timings
{
    constexpr uint32_t MODE_SWITCH_PENALTY = 1;
    constexpr uint32_t OP2_SHIFTED_REGISTER_PENALTY = 1;
    constexpr uint32_t LOAD_BASE_COST = 1;
    constexpr uint32_t SWAP_BASE_COST = 1;
}

uint32_t CalculateMultiplierCycles(uint32_t rsValue);
