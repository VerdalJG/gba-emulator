#include "Core/GBA_WaitstateController.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/GBA_Memory_Helpers.hpp"

void GBA_WaitstateController::SetWaitstateConfig(uint16_t waitcnt)
{
    // SRAM (bits 0-1): 00->4, 01->3, 10->2, 11->8
    static constexpr int sramMap[4] = {4, 3, 2, 8};
    sram_cycles = sramMap[waitcnt & 0b11];

    // WS0 first (bits 2-3): 00->4,01->3,10->2,11->8
    nonSequentialAccess[0] = sramMap[(waitcnt >> 2) & 0b11];

    // WS0 second (bit 4): 0->2, 1->1
    sequentialAccess[0] = ((waitcnt >> 4) & 0b1) ? 1 : 2;

    // WS1 first (bits 5-6)
    nonSequentialAccess[1] = sramMap[(waitcnt >> 5) & 0b11];

    // WS1 second (bit 7): 0->4, 1->1 (special mapping)
    sequentialAccess[1] = ((waitcnt >> 7) & 0b1) ? 1 : 4;

    // WS2 first (bits 8-9)
    nonSequentialAccess[2] = sramMap[(waitcnt >> 8) & 0b11];

    // WS2 second (bit 10): 0->8, 1->1 (special mapping)
    sequentialAccess[2] = ((waitcnt >> 10) & 0b1) ? 1 : 8;

    // Prefetch (bit 14)
    prefetchEnabled = (waitcnt >> 14) & 1;
}


int GBA_WaitstateController::GetCycles(uint32_t address, AccessSize size, bool isSequential) const
{
    if (address >= SRAM_START) return 1 + sram_cycles;
    if (address >= ROM0_START)
    {
        int romRegion;
        if (address >= ROM2_START) romRegion = 2;
        else if (address >= ROM1_START) romRegion = 1;
        else if (address >= ROM0_START) romRegion = 0;

        if (size == AccessSize::Word)
        {
            return 1 + sequentialAccess[romRegion] + nonSequentialAccess[romRegion];
        }
        else
        {
            int cycles = isSequential ? sequentialAccess[romRegion] : nonSequentialAccess[romRegion];
            return 1 + cycles;
        }
    }

    if (address >= EWRAM_START && address < IWRAM_START) 
    {
        switch (size)
        {
            case AccessSize::Byte: case AccessSize::Halfword: return 3;
            case AccessSize::Word: return 6;
        }
    }
    
    // Fast internal regions default to 1 cycle (1 + 0 waitstates)
    return 1;
}

