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


int GBA_WaitstateController::GetCycles(RegionType region, bool isSequential) const
{
    switch (region)
    {
        case RegionType::SRAM:
            return 1 + sram_cycles;

        case RegionType::ROM0:
            return 1 + (isSequential ? sequentialAccess[0] : nonSequentialAccess[0]);

        case RegionType::ROM1:
            return 1 + (isSequential ? sequentialAccess[1] : nonSequentialAccess[1]);

        case RegionType::ROM2:
            return 1 + (isSequential ? sequentialAccess[2] : nonSequentialAccess[2]);

        case RegionType::EWRAM:
            return 3;

        case RegionType::OAM:
        case RegionType::VRAM:
        case RegionType::PaletteRAM:
            return 1; // TODO: Add +1 cycle if GBA accesses video memory at the same time

        case RegionType::Invalid:
            return 0;

        default:
            return 1;
    }
}

/*

TODO: Prefetch behavior:

What ROM prefetch actually does (hardware reality)

When WAITCNT.PREFETCH = 1:
The cartridge bus continues fetching sequential 16-bit halfwords
These halfwords are placed into a small prefetch buffer (effectively a FIFO)
Instruction fetches can be satisfied from this buffer without incurring ROM waitstates

Critical properties:
1. Prefetch only applies to ROM
2. Prefetch only applies to instruction fetches
3. Prefetch only works for sequential addresses
4. Any non-ROM access flushes the buffer
5. Any non-sequential ROM access flushes the buffer

May have to add access type (instruction fetch vs data access)
*/

/*

Different version of GetCycle():


if (region == GBA_MemoryRegionType::SRAM) return 1 + sram_cycles;

    bool isRomRegion =  region == GBA_MemoryRegionType::ROM0 ||
                        region == GBA_MemoryRegionType::ROM1 || 
                        region == GBA_MemoryRegionType::ROM2;
    if (isRomRegion)
    {
        uint32_t romRegion = static_cast<uint32_t>(region) - static_cast<uint32_t>(GBA_MemoryRegionType::ROM0);
        if (size == BusAccessSize::Word)
        {
            return 1 + sequentialAccess[romRegion] + nonSequentialAccess[romRegion];
        }
        else
        {
            return 1 + (isSequential ? sequentialAccess[romRegion] : nonSequentialAccess[romRegion]);
        }
    }

    if (region == GBA_MemoryRegionType::EWRAM)
    {
        switch (size)
        {
            case BusAccessSize::Byte: case BusAccessSize::Halfword: return 3;
            case BusAccessSize::Word: return 6;
        }
    }

    if (region == GBA_MemoryRegionType::PaletteRAM || region == GBA_MemoryRegionType::VRAM)
    {
        switch (size)
        {
            case BusAccessSize::Byte: case BusAccessSize::Halfword: return 1;
            case BusAccessSize::Word: return 2;
        }
    }

    return 1;

*/

