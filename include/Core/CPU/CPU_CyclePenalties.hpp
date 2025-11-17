#pragma once

namespace GBA_Timings
{
    constexpr int PIPELINE_FLUSH_PENALTY = 2;
    constexpr int MODE_SWITCH_PENALTY = 1;
    constexpr int OP2_SHIFTED_REGISTER_PENALTY = 1;
    constexpr int DATA_PROCESSING_COST = 1;
}
