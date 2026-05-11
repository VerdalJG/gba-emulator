#include "Core/IO/Registers/SystemControlRegisters.hpp"

int WaitstateControl::GetCycles(RegionType region, bool isSequential) const
{
    static constexpr int sramMap[4] = {4, 3, 2, 8};
    static constexpr int nonsequentialMap[4] = {4, 3, 2, 8};
    static constexpr int sequentialWS0Map[2] = {2, 1};
    static constexpr int sequentialWS1Map[2] = {4, 1};
    static constexpr int sequentialWS2Map[2] = {8, 1};

    switch (region)
    {
        case RegionType::SRAM:
            return 1 + sramMap[fields.sramWaitControl];

        case RegionType::ROM0:
            return 1 + (isSequential 
                ? sequentialWS0Map[fields.waitstate0SecondAccess] 
                : nonsequentialMap[fields.waitstate0FirstAccess]);

        case RegionType::ROM1:
            return 1 + (isSequential 
                ? sequentialWS1Map[fields.waitstate1SecondAccess] 
                : nonsequentialMap[fields.waitstate1FirstAccess]);

        case RegionType::ROM2:
            return 1 + (isSequential 
                ? sequentialWS2Map[fields.waitstate2SecondAccess] 
                : nonsequentialMap[fields.waitstate2FirstAccess]);

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

What ROM prefetch actually does (hardware)

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
