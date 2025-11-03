#pragma once
#include <cstdint>

enum class AccessSize
{
    Byte = 1,
    Halfword = 2,
    Word = 4
};

class WaitstateController
{
public:
    // Call this when WAITCNT (0x04000204) is written
    void SetWaitstateConfig(uint16_t waitcnt);

    // Returns total cycles (1 + waitstates) for this access.
    // `isSequential` is whether the access is sequential (S) or non-sequential (N).
    int GetCycles(uint32_t address, AccessSize size, bool isSequential) const;

    bool IsPrefetchEnabled() const { return prefetchEnabled; }

private:
    int firstAccess[3]  = {4,4,4}; // default mapping
    int secondAccess[3] = {2,4,8}; // default mapping per GBATEK defaults
    int sram_cycles = 8;
    bool prefetchEnabled = true;
};