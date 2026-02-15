#pragma once
#include <cstdint>

#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_WaitstateController.hpp"

struct LastBusAccess
{
    uint32_t address = 0;
    GBA_MemoryRegionType region = GBA_MemoryRegionType::Invalid;
    BusAccessSize size = BusAccessSize::Invalid;
    bool advancesBus = false;
};

enum class BusRequester
{
    CPU,
    PPU,
    APU,
    DMA
};

enum Access : uint
{
    Nonsequential = 0,
    Sequential = 1,
    Code = 2, // Instruction fetch/prefetch
    Data = 4, // CPU Load/Store
    DMA = 8,  // DMA bus request
    Lock = 16
};

class EmulatorCore;
class GBA_Memory;
class GBA_PPU;
class GBA_APU;
class GBA_DMAController;

class GBA_Bus
{
public:
    GBA_Bus(EmulatorCore* core, GBA_Memory& memory);

    void AttachSubsystems(GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma);

    uint8_t Read8(uint32_t address, BusRequester requester, uint32_t* cycles);
    uint16_t Read16(uint32_t address, BusRequester requester, uint32_t* cycles);
    uint32_t Read32(uint32_t address, BusRequester requester, uint32_t* cycles);
    
    void Write8(uint32_t address, uint8_t value, BusRequester requester, uint32_t* cycles);
    void Write16(uint32_t address, uint16_t value, BusRequester requester, uint32_t* cycles);
    void Write32(uint32_t address, uint32_t value, BusRequester requester, uint32_t* cycles);

    // Open bus tracking
    void UpdateLatestAccessValues(uint32_t address, GBA_MemoryRegionType regionType, BusAccessSize accessSize, bool isValid);
    bool IsSequential(uint32_t address, BusAccessSize size, GBA_MemoryRegionType region);

    /* Sequentiality is only invalidated upon:
    1. Pipeline refill
    2. Branch taken
    3. Exception entry
    4. Mode change that refills the pipeline
    5. Any other event that breaks linear bus flow
    */ 
    void InvalidateSequentiality();

    GBA_WaitstateController& GetWaitstateController() { return waitstateController; }

private:
    // This is done because ROM0 uses the same bus as ROM1 and ROM2, preventing adjacent accesses
    // across the two waitstate regions from being treated as non-sequential
    BusDomain GetBusDomain(GBA_MemoryRegionType region) const;

    // Last value read from the bus (for open bus behavior)
    uint32_t lastValue = 0;
    LastBusAccess lastAccess;
    
    EmulatorCore* core;
    GBA_PPU* ppu;
    GBA_APU* apu;
    GBA_DMAController* dma;
    GBA_Memory& memory;
    GBA_WaitstateController waitstateController;
};