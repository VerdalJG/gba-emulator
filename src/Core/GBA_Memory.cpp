#include "Core/Memory/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_ROM.hpp"
#include "Core/GBA_IO.hpp"

#include <stdexcept>
#include <assert.h>


GBA_Memory::GBA_Memory(EmulatorCore *core, GBA_ROM& rom)
    : core(core), rom(rom), 
    bios(std::make_unique<std::vector<u8>>(BIOS_SIZE)), 
    ewram(std::make_unique<std::vector<u8>>(EWRAM_SIZE)), 
    iwram(std::make_unique<std::vector<u8>>(IWRAM_SIZE)), 
    paletteRam(std::make_unique<std::vector<u8>>(PALETTE_RAM_SIZE)), 
    vram(std::make_unique<std::vector<u8>>(VRAM_TOTAL_SIZE)), 
    oam(std::make_unique<std::vector<u8>>(OAM_SIZE)),
    sram(std::make_unique<std::vector<u8>>(SRAM_SIZE))
{}

const MemoryRegion* GBA_Memory::GetRegionFromAddress(u32 address) const
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


std::span<const u8> GBA_Memory::GetRegionData(RegionType type) const 
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

std::span<u8> GBA_Memory::GetRegionDataMutable(RegionType type) 
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

void GBA_Memory::InitROMBanks()
{
    const std::span<const u8>& romData = *rom.GetROMData();

    rom0View = romData;
    rom1View = romData;
    rom2View = romData;

    rom0Region.physicalSize = rom.GetSize();
    rom1Region.physicalSize = rom.GetSize();
    rom2Region.physicalSize = rom.GetSize();
}

void GBA_Memory::LoadBIOS(const std::vector<u8>& biosData)
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
    std::span<u8> regionData = GetRegionDataMutable(type); 
    std::fill(regionData.begin(), regionData.end(), 0);
}

void GBA_Memory::Clear8(u32 address)
{
    //RegionType region = GetRegionTypeFromAddress(address);
    //Write8(address, 0, region);
}

void GBA_Memory::Clear16(u32 address)
{
    //RegionType region = GetRegionTypeFromAddress(address);
    //Write16(address, 0, region);
}

void GBA_Memory::Clear32(u32 address)
{
    //RegionType region = GetRegionTypeFromAddress(address);
    //Write32(address, 0, region);
}

void GBA_Memory::ClearAddressRange(u32 startAddress, u32 endAddress)
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

    std::span<u8> regionData = GetRegionDataMutable(region->type); 

    u32 startOffset = startAddress - region->start;
    u32 endOffset = endAddress - region->start;

    assert(endOffset < regionData.size() && startOffset <= endOffset && "Attempting to clear more than one region");

    std::fill(regionData.begin() + startOffset, regionData.begin() + endOffset + 1, 0); 
}


