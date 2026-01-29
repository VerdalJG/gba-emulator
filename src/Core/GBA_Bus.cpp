#include "Core/GBA_Bus.hpp"
#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/GBA_PPU.hpp"
#include "Core/GBA_APU.hpp"
#include "Core/GBA_DMAController.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/EmulatorCore.hpp"


GBA_Bus::GBA_Bus(EmulatorCore* core, GBA_Memory& memory) : core(core), memory(memory)
{
    
}

void GBA_Bus::AttachSubsystems(GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma) 
{
    this->ppu = ppu;
    this->apu = apu;
    this->dma = dma;
}

uint8_t GBA_Bus::Read8(uint32_t address, BusRequester requester, uint32_t* cycles) 
{
    // Get the region
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (cycles) // DMA and CPU are the ones that control cycles
    {
        // Cycle calculation
        bool isSequential = IsSequential(address, BusAccessSize::Byte, regionType);
        *cycles = waitstateController.GetCycles(regionType, isSequential);

        // The CPU must wait 1 cycle if the ppu is currently accessing video memory
        if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
        {
            if (region->IsVideoMemory()) *cycles += 1;
        }
    }

    // Open-bus access
    if (!region || !region->IsValidAccess(address, AccessType::Read, BusAccessSize::Byte))
    {
        const std::string message = "Invalid Read8 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);
        
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Byte, false);
        return lastValue & 0xFF; // Return the low 8 bits
    }

    // Perform read
    MemReadResult<uint8_t> readResult = memory.Read8(address, regionType);
    uint8_t finalValue = 0;

    if (readResult.valid)
    {
        // Update last value
        lastValue = (lastValue & ~0xFFu) | readResult.value;
        finalValue = readResult.value;
    }
    else // Unused address detected
    {
        // Open-bus
        finalValue = lastValue & 0xFF;
    }

    UpdateLatestAccessValues(address, regionType, BusAccessSize::Byte, readResult.valid);
    return finalValue;
}

uint16_t GBA_Bus::Read16(uint32_t address, BusRequester requester, uint32_t* cycles) 
{ 
    // Get the region
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (cycles) // DMA and CPU are the ones that control cycles
    {
        // Cycle calculation
        bool isSequential = IsSequential(address, BusAccessSize::Halfword, regionType);
        *cycles = waitstateController.GetCycles(regionType, isSequential);

        // The CPU must wait 1 cycle if the ppu is currently accessing video memory
        if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
        {
            if (region->IsVideoMemory()) *cycles += 1;
        }
    }

    // Open-bus access
    if (!region || !region->IsValidAccess(address, AccessType::Read, BusAccessSize::Halfword))
    {
        const std::string message = "Invalid Read16 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);
        
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, false);
        return lastValue & 0xFFFF; // Return the low 16 bits
    }

    // Perform read
    MemReadResult<uint16_t> readResult = memory.Read16(address, regionType);
    uint16_t finalValue = 0;

    if (readResult.valid)
    {
        // Update last value
        lastValue = (lastValue & ~0xFFFFu) | readResult.value;
        finalValue = readResult.value;
    }
    else // Unused address detected
    {
        // Open-bus
        finalValue = lastValue & 0xFFFF;
    }

    UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, readResult.valid);
    return finalValue;
}

uint32_t GBA_Bus::Read32(uint32_t address, BusRequester requester, uint32_t* cycles) 
{ 
    // Get the region
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    // Open-bus access
    if (!region || !region->IsValidAccess(address, AccessType::Read, BusAccessSize::Word))
    {
        const std::string message = "Invalid Read32 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);

        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles = waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }
        return lastValue; // Return the full value
    }

    // Some regions have a 16-bit bus and need 2 accesses to perform a 32 bit read
    int accesses = region->AccessesRequired(BusAccessSize::Word);
    if (cycles) *cycles = 0;

    // -------- Case A: native 32-bit bus --------
    if (accesses == 1)
    {
        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles = waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        MemReadResult<uint32_t> readResult = memory.Read32(address, regionType);
        uint32_t finalValue = 0;

        if (readResult.valid)
        {
            // Update last value
            lastValue = readResult.value;
            finalValue = readResult.value;
        }
        else // Unused address detected
        {
            // Open-bus
            finalValue = lastValue;
        }

        UpdateLatestAccessValues(address, regionType, BusAccessSize::Word, readResult.valid);
        return finalValue;
    }

    // -------- Case B: 16-bit bus --------
    uint32_t finalValue = 0;

    // First halfword
    {
        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles += waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        MemReadResult<uint16_t> lowReadResult = memory.Read16(address, regionType);
        uint16_t low = 0;

        if (lowReadResult.valid)
        {
            // Update last value
            lastValue = (lastValue & 0xFFFF0000u) | lowReadResult.value;
            low = lowReadResult.value;
        }
        else // Unused address detected
        {
            // Open-bus
            low = lastValue & 0xFFFF;
        }
        finalValue |= low;
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, lowReadResult.valid);
    }

    // Second halfword
    {
        uint32_t address2 = address + 2;

        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles += waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        MemReadResult<uint16_t> highReadResult = memory.Read16(address2, regionType);
        uint16_t high = 0;

        if (highReadResult.valid)
        {
            // Update last value
            lastValue = (lastValue & 0x0000FFFFu) | (static_cast<uint32_t>(highReadResult.value) << 16);
            high = highReadResult.value;
        }
        else // Unused address detected
        {
            // Open-bus
            high = static_cast<uint16_t>((lastValue >> 16) & 0xFFFF);
        }
        finalValue |= static_cast<uint32_t>(high) << 16;
        UpdateLatestAccessValues(address2, regionType, BusAccessSize::Halfword, highReadResult.valid);
    }

    return finalValue;
}

void GBA_Bus::Write8(uint32_t address, uint8_t value, BusRequester requester, uint32_t* cycles) 
{
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (cycles) // DMA and CPU are the ones that control cycles
    {
        // Cycle calculation
        bool isSequential = IsSequential(address, BusAccessSize::Byte, regionType);
        *cycles = waitstateController.GetCycles(regionType, isSequential);

        // The CPU must wait 1 cycle if the ppu is currently accessing video memory
        if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
        {
            if (region->IsVideoMemory()) *cycles += 1;
        }
    }

    // Drive the bus
    lastValue = (lastValue & ~0xFFu) | value;

    if (region && region->IsValidAccess(address, AccessType::Write, BusAccessSize::Byte))
    {
        memory.Write8(address, value, regionType);
    }

    UpdateLatestAccessValues(address, regionType, BusAccessSize::Byte, true);
}

void GBA_Bus::Write16(uint32_t address, uint16_t value, BusRequester requester, uint32_t* cycles) 
{
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (cycles) // DMA and CPU are the ones that control cycles
    {
        // Cycle calculation
        bool isSequential = IsSequential(address, BusAccessSize::Halfword, regionType);
        *cycles = waitstateController.GetCycles(regionType, isSequential);

        // The CPU must wait 1 cycle if the ppu is currently accessing video memory
        if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
        {
            if (region->IsVideoMemory()) *cycles += 1;
        }
    }

    // Drive the bus
    lastValue = (lastValue & ~0xFFFFu) | value;

    if (region && region->IsValidAccess(address, AccessType::Write, BusAccessSize::Halfword))
    {
        memory.Write16(address, value, regionType);
    }

    UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, true);
}

void GBA_Bus::Write32(uint32_t address, uint32_t value, BusRequester requester, uint32_t* cycles) 
{
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (!region || !region->IsValidAccess(address, AccessType::Write, BusAccessSize::Word))
    {
        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles = waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        lastValue = value;
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Word, true);
        return;
    }

    int accesses = region->AccessesRequired(BusAccessSize::Word);
    if (cycles) *cycles = 0;

    // -------- Case A: native 32-bit bus --------
    if (accesses == 1)
    {
        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles = waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        lastValue = value;
        memory.Write32(address, value, regionType);

        UpdateLatestAccessValues(address, regionType, BusAccessSize::Word, true);
        return;
    }

    // -------- Case B: 16-bit bus --------
    // First halfword
    {
        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles += waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        uint16_t low = value & 0xFFFF;
        lastValue = (lastValue & 0xFFFF0000u) | low;

        memory.Write16(address, low, regionType);
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, true);
    }

    // Second halfword
    {
        uint32_t address2 = address + 2;

        if (cycles) // DMA and CPU are the ones that control cycles
        {
            // Cycle calculation
            bool isSequential = IsSequential(address, BusAccessSize::Word, regionType);
            *cycles += waitstateController.GetCycles(regionType, isSequential);

            // The CPU must wait 1 cycle if the ppu is currently accessing video memory
            if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
            {
                if (region->IsVideoMemory()) *cycles += 1;
            }
        }

        uint16_t high = value >> 16;
        lastValue = (lastValue & 0x0000FFFFu) | (static_cast<uint32_t>(high) << 16);

        memory.Write16(address2, high, regionType);
        UpdateLatestAccessValues(address2, regionType, BusAccessSize::Halfword, true);
    }
}

void GBA_Bus::UpdateLatestAccessValues(uint32_t address, 
    GBA_MemoryRegionType region, BusAccessSize accessSize, bool isValid) 
{
    lastAccess.address = address;
    lastAccess.region = region;
    lastAccess.size = accessSize;
    lastAccess.advancesBus = isValid;
}

bool GBA_Bus::IsSequential(uint32_t address, BusAccessSize size, GBA_MemoryRegionType region) 
{ 
    if (!lastAccess.advancesBus)
        return false;

    if (GetBusDomain(lastAccess.region) != GetBusDomain(region))
        return false;

    if (lastAccess.size != size)
        return false;

    if (address != lastAccess.address + static_cast<size_t>(size))
        return false;

        // --- Game Pak ROM 128 KB boundary rule ---
    if (GetBusDomain(region) == BusDomain::GamePakROM)
    {
        constexpr uint32_t BLOCK_MASK = ~0x1FFFFu; // 128 KB
        if ((address & BLOCK_MASK) != (lastAccess.address & BLOCK_MASK))
            return false;
    }

    return true;
}

void GBA_Bus::InvalidateSequentiality() 
{
    lastAccess.advancesBus = false;
}

BusDomain GBA_Bus::GetBusDomain(GBA_MemoryRegionType region) const
{
    switch (region)
    {
        case GBA_MemoryRegionType::ROM0:
        case GBA_MemoryRegionType::ROM1:
        case GBA_MemoryRegionType::ROM2:
            return BusDomain::GamePakROM;

        default:
            return BusDomain::Other;
    }
}
