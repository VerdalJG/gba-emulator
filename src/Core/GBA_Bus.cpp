#include "Core/GBA_Bus.hpp"
#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"


GBA_Bus::GBA_Bus(EmulatorCore* core, GBA_Memory& memory) : core(core), memory(memory)
{
    
}

uint8_t GBA_Bus::Read8(uint32_t address, uint32_t& cycles) 
{
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);
    uint8_t readValue = 0;

    bool isSequential = IsSequential(address, BusAccessSize::Byte, regionType);
    cycles = waitstateController.GetCycles(regionType, isSequential);

    if (region && region->IsValidAccess(address, AccessType::Read, BusAccessSize::Byte))
    {
        readValue = memory.Read8(address, regionType);

        // Update last value
        uint32_t merged = lastValue;
        merged = (merged & ~0xFFu) | readValue;
        lastValue = merged;
    }
    else // Open-bus access
    {
        const std::string message = "Invalid Read8 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);
        readValue = lastValue & 0xFF; // Return the low 8 bits
    }
    
    UpdateLatestAccessValues(address, regionType, BusAccessSize::Byte, true);
    return readValue;
}

uint16_t GBA_Bus::Read16(uint32_t address, uint32_t& cycles) 
{ 
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);
    uint16_t readValue = 0;

    bool isSequential = IsSequential(address, BusAccessSize::Halfword, regionType);
    cycles = waitstateController.GetCycles(regionType, isSequential);

    if (region && region->IsValidAccess(address, AccessType::Read, BusAccessSize::Halfword))
    {
        readValue = memory.Read16(address, regionType);

        // Update last value
        uint32_t merged = lastValue;
        merged = (merged & ~0xFFFFu) | readValue;
        lastValue = merged;
    }
    else // Open-bus access
    {
        const std::string message = "Invalid Read16 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);
        readValue = lastValue & 0xFFFF; // Return the low 16 bits
    }
    
    UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, true);
    return readValue;
}

uint32_t GBA_Bus::Read32(uint32_t address, uint32_t& cycles) 
{ 
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);
    uint32_t readValue = 0;

    // Open-bus access
    if (!region || !region->IsValidAccess(address, AccessType::Read, BusAccessSize::Word))
    {
        const std::string message = "Invalid Read32 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);

        cycles = waitstateController.GetCycles(regionType, false);
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, false);
        return lastValue; // Return the full value
    }

    int accesses = region->AccessesRequired(BusAccessSize::Word);

    // -------- Case A: native 32-bit bus --------
    if (accesses == 1)
    {
        bool sequential = IsSequential(address, BusAccessSize::Word, regionType);
        cycles = waitstateController.GetCycles(regionType, sequential);

        readValue = memory.Read32(address, regionType);
        lastValue = readValue;

        UpdateLatestAccessValues(address, regionType, BusAccessSize::Word, true);
        return readValue;
    }

    // -------- Case B: 16-bit bus --------

    // First halfword
    {
        bool sequential = IsSequential(address, BusAccessSize::Halfword, regionType);
        cycles += waitstateController.GetCycles(regionType, sequential);

        uint16_t low = memory.Read16(address, regionType);
        readValue |= low;

        lastValue = (lastValue & 0xFFFF0000u) | low;
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, true);
    }

    // Second halfword
    {
        uint32_t address2 = address + 2;

        bool sequential;
        if (GetBusDomain(regionType) == BusDomain::GamePakROM)
        {
            // ROM regions rule: always sequential
            sequential = true;
        }
        else
        {
            sequential = IsSequential(address2, BusAccessSize::Halfword, regionType);
        }

        cycles += waitstateController.GetCycles(regionType, sequential);

        uint16_t high = memory.Read16(address2, regionType);
        readValue |= static_cast<uint32_t>(high) << 16;

        lastValue = (lastValue & 0x0000FFFFu) | (static_cast<uint32_t>(high) << 16);
        UpdateLatestAccessValues(address2, regionType, BusAccessSize::Halfword, true);
    }

    return readValue;
}

void GBA_Bus::Write8(uint32_t address, uint8_t value, uint32_t& cycles) 
{
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    bool sequential = IsSequential(address, BusAccessSize::Byte, regionType);
    cycles = waitstateController.GetCycles(regionType, sequential);

    // Drive the bus
    lastValue = (lastValue & ~0xFFu) | value;

    if (region && region->IsValidAccess(address, AccessType::Write, BusAccessSize::Byte))
    {
        memory.Write8(address, value, regionType);
    }

    UpdateLatestAccessValues(address, regionType, BusAccessSize::Byte, true);
}

void GBA_Bus::Write16(uint32_t address, uint16_t value, uint32_t& cycles) 
{
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    bool sequential = IsSequential(address, BusAccessSize::Halfword, regionType);
    cycles = waitstateController.GetCycles(regionType, sequential);

    lastValue = (lastValue & ~0xFFFFu) | value;

    if (region && region->IsValidAccess(address, AccessType::Write, BusAccessSize::Halfword))
    {
        memory.Write16(address, value, regionType);
    }

    UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, true);
}

void GBA_Bus::Write32(uint32_t address, uint32_t value, uint32_t& cycles) 
{
    GBA_MemoryRegionType regionType = memory.GetRegionTypeFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (!region || !region->IsValidAccess(address, AccessType::Write, BusAccessSize::Word))
    {
        cycles = waitstateController.GetCycles(regionType, false);
        lastValue = value;
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Word, false);
        return;
    }

    int accesses = region->AccessesRequired(BusAccessSize::Word);

    // -------- Case A: native 32-bit bus --------
    if (accesses == 1)
    {
        bool sequential = IsSequential(address, BusAccessSize::Word, regionType);
        cycles = waitstateController.GetCycles(regionType, sequential);

        lastValue = value;
        memory.Write32(address, value, regionType);

        UpdateLatestAccessValues(address, regionType, BusAccessSize::Word, true);
        return;
    }

    // -------- Case B: 16-bit bus --------
    // First halfword
    {
        bool sequential = IsSequential(address, BusAccessSize::Halfword, regionType);
        cycles += waitstateController.GetCycles(regionType, sequential);

        uint16_t low = value & 0xFFFF;
        lastValue = (lastValue & 0xFFFF0000u) | low;

        memory.Write16(address, low, regionType);
        UpdateLatestAccessValues(address, regionType, BusAccessSize::Halfword, true);
    }

    // Second halfword
    {
        uint32_t address2 = address + 2;

        bool sequential =
            (GetBusDomain(regionType) == BusDomain::GamePakROM)
                ? true
                : IsSequential(address2, BusAccessSize::Halfword, regionType);

        cycles += waitstateController.GetCycles(regionType, sequential);

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
    lastAccess.valid = isValid;
}

bool GBA_Bus::IsSequential(uint32_t address, BusAccessSize size, GBA_MemoryRegionType region) 
{ 
    if (!lastAccess.valid)
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
    lastAccess.valid = false;
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
