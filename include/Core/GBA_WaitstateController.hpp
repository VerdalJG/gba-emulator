#pragma once
#include <cstdint>



class GBA_WaitstateController
{
public:
    // Call this when WAITCNT (0x04000204) is written
    void SetWaitstateConfig(uint16_t waitcnt);

    // Returns total cycles (1 + waitstates) for this access.
    // isSequential is whether the access is sequential (S) or non-sequential (N).
    int GetCycles(GBA_MemoryRegionType region, bool isSequential) const;

    bool IsPrefetchEnabled() const { return prefetchEnabled; }

private:
    int nonSequentialAccess[3]  = {4,4,4};
    int sequentialAccess[3] = {2,4,8}; // From GBATEK defaults
    int sram_cycles = 5;
    bool prefetchEnabled = true;
};