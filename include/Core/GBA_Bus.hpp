#pragma once
#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_WaitstateController.hpp"

#include "Utils/Integers.hpp"

struct LastBusAccess
{
    uint32_t address = 0;
    GBA_MemoryRegionType region = GBA_MemoryRegionType::Invalid;
    size_t size = size_t::Invalid;
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

    template <typename T>
    T Read(u32 address, BusRequester requester, u32* cycles)
    {
        // Get the region
        GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
        const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, sizeof(T), regionType);
            *cycles = waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        // Open-bus access
        if (!region || !region->IsValidAccess<T>(address))
        {
            // Return open bus
        }

        // Perform read
        MemReadResult<T> readResult = memory.Read8(address, regionType);
        T finalValue = 0;

        if (readResult.valid)
        {
            // Update last value
            lastValue = (lastValue & ~0xFFFFu) | readResult.value;
            finalValue = readResult.value;
        }
        else // Unused address detected
        {
            // Open-bus
        }

        UpdateLatestAccessValues(address, regionType, size_t::Halfword, readResult.valid);
        return finalValue;
    }

    template <typename T>
    void Write(u32 address, T value, BusRequester requester, u32* cycles)
    {

    }

    // Open bus tracking
    void UpdateLatestAccessValues(uint32_t address, GBA_MemoryRegionType regionType, size_t accessSize, bool isValid);
    bool IsSequential(uint32_t address, size_t size, GBA_MemoryRegionType region);

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

    template <typename T>
    T ReturnOpenBus()
    {

    }
    
    EmulatorCore* core;
    GBA_PPU* ppu;
    GBA_APU* apu;
    GBA_DMAController* dma;
    GBA_Memory& memory;
    GBA_WaitstateController waitstateController;
};

// #include "MemoryAccess.inl"