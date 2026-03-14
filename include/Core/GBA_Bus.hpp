#pragma once
#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_WaitstateController.hpp"

#include "Utils/Integers.hpp"

struct LastBusAccess
{
    u32 value;
    u32 address = 0;
    RegionType region = RegionType::Invalid;
    size_t size = AccessSize::Invalid;
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
class GBA_IO;
class GBA_CPU;

class GBA_Bus
{
public:
    GBA_Bus(EmulatorCore* core, GBA_Memory& memory, GBA_IO& io);

    void AttachSubsystems(GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma, GBA_CPU* cpu);

    template <typename T>
    T Read(u32 address, BusRequester requester);

    template <typename T>
    void Write(u32 address, T value, BusRequester requester);

    // Open bus tracking
    void UpdateLatestAccessValues(u32 value, u32 address, RegionType regionType, AccessSize accessSize);
    bool IsSequential(u32 address, AccessSize size, RegionType region);

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
    BusDomain GetBusDomain(RegionType region) const;

    // Last bus access data (for open bus behavior)
    LastBusAccess lastAccess;
    bool accessForcedNonSequential = false;

    // These regions have special behavior when reading/writing
    template <typename T>
    T ReadBIOS(u32 address);

    template <typename T>
    T ReadPaletteRAM(u32 address, BusRequester requester);

    template <typename T>
    T ReadVRAM(u32 address, BusRequester requester);

    template <typename T>
    T ReadOAM(u32 address, BusRequester requester);
    
    template <typename T>
    T ReadSRAM(u32 address);

    

    u32 OpenBus(u32 address);
    void HandleAccessCycles(u32 address, MemoryRegion* region, AccessSize size, uint accesses, BusRequester requester);
    u32 ComputeAccessOffset(u32 address, MemoryRegion* type);
    const RegionType GetRegionType(u32 address) const;

    u32 biosLatch = 0;
    
    EmulatorCore* core;
    GBA_CPU* cpu;
    GBA_PPU* ppu;
    GBA_APU* apu;
    GBA_DMAController* dma;
    GBA_Memory& memory;
    GBA_IO& io;
    GBA_WaitstateController waitstateController;
};

#include "MemoryAccess.tpp"