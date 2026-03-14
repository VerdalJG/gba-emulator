#include "Core/Memory/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_ROM.hpp"
#include "Core/GBA_IO.hpp"

#include <stdexcept>
#include <assert.h>


GBA_Memory::GBA_Memory(EmulatorCore *core, GBA_ROM& rom)
    : core(core), rom(rom), 
    bios(std::make_unique<std::vector<uint8_t>>(BIOS_SIZE)), 
    ewram(std::make_unique<std::vector<uint8_t>>(EWRAM_SIZE)), 
    iwram(std::make_unique<std::vector<uint8_t>>(IWRAM_SIZE)), 
    paletteRam(std::make_unique<std::vector<uint8_t>>(PALETTE_RAM_SIZE)), 
    vram(std::make_unique<std::vector<uint8_t>>(VRAM_TOTAL_SIZE)), 
    oam(std::make_unique<std::vector<uint8_t>>(OAM_SIZE)),
    sram(std::make_unique<std::vector<uint8_t>>(SRAM_SIZE))
{}

const MemoryRegion* GBA_Memory::GetRegionFromAddress(uint32_t address) const
{
    switch (address >> 24)
    {
        case 0x00: return &biosRegion;
        case 0x02: return &ewramRegion;
        case 0x03: return &iwramRegion;
        case 0x04: return &ioRegion;
        case 0x05: return &paletteRamRegion;
        case 0x06: return &vramRegion;
        case 0x07: return &oamRegion;
        case 0x08: case 0x09: return &rom0Region;
        case 0x0A: case 0x0B: return &rom1Region;
        case 0x0C: case 0x0D: return &rom2Region;
        case 0x0E: return &sramRegion;

        default: return nullptr; // Unused or invalid address
    }
}

const MemoryRegion* GBA_Memory::GetRegionFromType(RegionType type) const
{
    switch (type)
    {
        case RegionType::BIOS: return &biosRegion;
        case RegionType::EWRAM: return &ewramRegion;
        case RegionType::IWRAM: return &iwramRegion;
        case RegionType::IO: return &ioRegion;
        case RegionType::PaletteRAM: return &paletteRamRegion;
        case RegionType::VRAM: return &vramRegion;
        case RegionType::OAM: return &oamRegion;
        case RegionType::ROM0: return &rom0Region;
        case RegionType::ROM1: return &rom1Region;
        case RegionType::ROM2: return &rom2Region;
        case RegionType::SRAM: return &sramRegion;
        case RegionType::Invalid: return nullptr;

        default: return nullptr;
    }
}


std::span<const uint8_t> GBA_Memory::GetRegionData(RegionType type) const 
{
    switch (type)
    {
        case RegionType::BIOS: return *bios;
        case RegionType::EWRAM: return *ewram;
        case RegionType::IWRAM: return *iwram;
        case RegionType::PaletteRAM: return *paletteRam;
        case RegionType::VRAM: return *vram;
        case RegionType::OAM: return *oam;
        case RegionType::ROM0: return rom0View;
        case RegionType::ROM1: return rom1View;
        case RegionType::ROM2: return rom2View;
        case RegionType::SRAM: return *sram;

        default: return {}; // Empty span
    }
}

std::span<uint8_t> GBA_Memory::GetRegionDataMutable(RegionType type) 
{
    switch (type)
    {
        case RegionType::BIOS: return *bios;
        case RegionType::EWRAM: return *ewram;
        case RegionType::IWRAM: return *iwram;
        case RegionType::PaletteRAM: return *paletteRam;
        case RegionType::VRAM: return *vram;
        case RegionType::OAM: return *oam;
        case RegionType::ROM0: return {}; // ROM is not mutable
        case RegionType::ROM1: return {}; // ROM is not mutable
        case RegionType::ROM2: return {}; // ROM is not mutable
        case RegionType::SRAM: return *sram;

        default: return {}; // Empty span
    }
}

uint32_t GBA_Memory::ComputeAccessOffset(uint32_t address, RegionType type) 
{
    uint32_t offset = 0;
    const MemoryRegion* region = GetRegionFromType(type);
    if (region->mirroring == Mirroring::Mirror) // EWRAM, IWRAM, OAM, Palette RAM, 
    {
        offset = (address - region->start) & (region->physicalSize - 1);
    }
    else if (region->mirroring == Mirroring::NoMirror) // ROM, BIOS
    {
        offset = (address - region->start);
    }
    else // SpecialMirror (VRAM, SRAM, IO) - IO mirroring handled inside IO class
    {
        if (type == RegionType::VRAM)
        {
            uint32_t relativeAddress = (address - VRAM_START);

            // Mirror every 128 KB (64KB + 32KB + 32KB(mirror of first 32KB))
            uint32_t windowOffset = relativeAddress & (VRAM_MIRROR_SIZE - 1);

            // Handle 32KB mirror case inside of the 128KB
            if (windowOffset >= VRAM_TOTAL_SIZE)
            {
                // Mirror OBJ VRAM (-0x8000)
                windowOffset -= VRAM_OBJ_SIZE;
            }

            // Now it is guranteed to be within 0x18000 window
            offset = windowOffset;
        }

        if (type == RegionType::SRAM)
        {
            uint32_t relativeAddress = address - SRAM_START;
            
            // Mirror across 32MB region
            offset = relativeAddress & (SRAM_SIZE - 1);

            // Mirror upper 32KB onto the lower 32KB
            offset &= (SRAM_MIRROR_SIZE - 1);
        }
    }

    return offset;
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
MemReadResult<uint8_t> GBA_Memory::Read8(uint32_t address, RegionType regionType)
{
    uint32_t offset = ComputeAccessOffset(address, regionType);
    const MemoryRegion* region = GetRegionFromType(regionType);
    
    if (offset >= region->physicalSize)
    {
        return OPEN_BUS;
    }

    std::span<const uint8_t> regionData = GetRegionData(regionType);
    return { regionData[offset], true };
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
MemReadResult<uint16_t> GBA_Memory::Read16(uint32_t address, RegionType regionType)
{  
    uint32_t offset = ComputeAccessOffset(address, regionType);
    const MemoryRegion* region = GetRegionFromType(regionType);
    
    if (offset + (sizeof(uint16_t) - 1) >= region->physicalSize)
    {
        return OPEN_BUS;
    }

    std::span<const uint8_t> regionData = GetRegionData(regionType);
    uint16_t readValue = 
        static_cast<uint16_t>(regionData[offset]) | 
        static_cast<uint16_t>(regionData[offset + 1]) << 8;

    return { readValue, true };
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
MemReadResult<uint32_t> GBA_Memory::Read32(uint32_t address, RegionType regionType)
{
    uint32_t offset = ComputeAccessOffset(address, regionType);
    const MemoryRegion* region = GetRegionFromType(regionType);
    
    if (offset + (sizeof(uint32_t) - 1) >= region->physicalSize)
    {
        return OPEN_BUS;
    }

    std::span<const uint8_t> regionData = GetRegionData(regionType);
    uint32_t readValue = 
        static_cast<uint32_t>(regionData[offset]) |
        static_cast<uint32_t>(regionData[offset + 1]) << 8  |
        static_cast<uint32_t>(regionData[offset + 2]) << 16 |
        static_cast<uint32_t>(regionData[offset + 3]) << 24;

    return { readValue, true };
}   

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
void GBA_Memory::Write8(uint32_t address, uint8_t value, RegionType regionType)
{
    uint32_t offset = ComputeAccessOffset(address, regionType);
    const MemoryRegion* region = GetRegionFromType(regionType);
    
    if (offset >= region->physicalSize) return;

    std::span<uint8_t> regionData = GetRegionDataMutable(regionType);
    regionData[offset] = value;
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
void GBA_Memory::Write16(uint32_t address, uint16_t value, RegionType regionType)
{
    uint32_t offset = ComputeAccessOffset(address, regionType);
    const MemoryRegion* region = GetRegionFromType(regionType);
    
    if (offset + (sizeof(uint16_t) - 1) >= region->physicalSize) return;

    std::span<uint8_t> regionData = GetRegionDataMutable(regionType);
    regionData[offset] = static_cast<uint8_t>(value & 0xFF);
    regionData[offset + 1] = static_cast<uint8_t>(value >> 8);
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
void GBA_Memory::Write32(uint32_t address, uint32_t value, RegionType regionType)
{
    uint32_t offset = ComputeAccessOffset(address, regionType);
    const MemoryRegion* region = GetRegionFromType(regionType);
    
    if (offset + (sizeof(uint32_t) - 1) >= region->physicalSize) return;

    std::span<uint8_t> regionData = GetRegionDataMutable(regionType);
    regionData[offset] = static_cast<uint8_t>(value & 0xFF);
    regionData[offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    regionData[offset + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    regionData[offset + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

void GBA_Memory::InitROMBanks()
{
    const std::span<const uint8_t>& romData = *rom.GetROMData();

    rom0View = romData;
    rom1View = romData;
    rom2View = romData;

    rom0Region.physicalSize = rom.GetSize();
    rom1Region.physicalSize = rom.GetSize();
    rom2Region.physicalSize = rom.GetSize();
}

void GBA_Memory::LoadBIOS(const std::vector<uint8_t>& biosData)
{
    if (biosData.size() != BIOS_SIZE)
    {
        // Error: Invalid bios data size
        throw std::runtime_error("Invalid BIOS size: expected 16 KB");
    }

    std::copy(biosData.begin(), biosData.end(), bios->begin());
}

void GBA_Memory::ClearRegion(RegionType type)
{
    std::span<uint8_t> regionData = GetRegionDataMutable(type); 
    std::fill(regionData.begin(), regionData.end(), 0);
}

void GBA_Memory::Clear8(uint32_t address)
{
    //RegionType region = GetRegionTypeFromAddress(address);
    //Write8(address, 0, region);
}

void GBA_Memory::Clear16(uint32_t address)
{
    //RegionType region = GetRegionTypeFromAddress(address);
    //Write16(address, 0, region);
}

void GBA_Memory::Clear32(uint32_t address)
{
    //RegionType region = GetRegionTypeFromAddress(address);
    //Write32(address, 0, region);
}

void GBA_Memory::ClearAddressRange(uint32_t startAddress, uint32_t endAddress)
{
    assert(startAddress <= endAddress && "Start address must be lower than the end address");

    const MemoryRegion* region = GetRegionFromAddress(startAddress);
    assert(region && "Invalid start address");
    assert(region == GetRegionFromAddress(endAddress) && "Start address and end address must pertain to the same region");

    // Ensure region is writeable (cannot clear BIOS nor ROM)
    if (region->type == RegionType::BIOS) return;
    if (region->type == RegionType::IO) return;
    if (region->type == RegionType::ROM0) return;
    if (region->type == RegionType::ROM1) return;
    if (region->type == RegionType::ROM2) return;

    std::span<uint8_t> regionData = GetRegionDataMutable(region->type); 

    uint32_t startOffset = startAddress - region->start;
    uint32_t endOffset = endAddress - region->start;

    assert(endOffset < regionData.size() && startOffset <= endOffset && "Attempting to clear more than one region");

    std::fill(regionData.begin() + startOffset, regionData.begin() + endOffset + 1, 0); 
}


