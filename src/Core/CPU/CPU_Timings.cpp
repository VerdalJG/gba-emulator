#include "Core/CPU/CPU_Timings.hpp"

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