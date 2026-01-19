#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_ROM.hpp"
#include "Core/GBA_IO.hpp"

#include <stdexcept>
#include <assert.h>


GBA_Memory::GBA_Memory(EmulatorCore *core, GBA_ROM& rom, GBA_IO& io)
    : core(core), rom(rom), io(io), 
    bios(std::make_unique<std::vector<uint8_t>>(BIOS_SIZE)), 
    ewram(std::make_unique<std::vector<uint8_t>>(EWRAM_SIZE)), 
    iwram(std::make_unique<std::vector<uint8_t>>(IWRAM_SIZE)), 
    paletteRam(std::make_unique<std::vector<uint8_t>>(PALETTE_RAM_SIZE)), 
    vram(std::make_unique<std::vector<uint8_t>>(VRAM_SIZE)), 
    oam(std::make_unique<std::vector<uint8_t>>(OAM_SIZE)), 
    rom0(std::make_unique<std::vector<uint8_t>>(ROM_BANK_SIZE)),
    rom1(std::make_unique<std::vector<uint8_t>>(ROM_BANK_SIZE)),
    rom2(std::make_unique<std::vector<uint8_t>>(ROM_BANK_SIZE)),
    sram(std::make_unique<std::vector<uint8_t>>(SRAM_SIZE))
{}

const GBA_MemoryRegion* GBA_Memory::GetRegionFromAddress(uint32_t address) const
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

        default: return nullptr; // Unused or invalid address, handle accordingly
    }

    // TODO:
    // Reading from BIOS region:

    // If reading from bios memory the GBA allows to read opcodes or data only if the program counter 
    // is located inside of the BIOS area. If the program counter is not in the BIOS area,
    // reading will return the most recent successfully fetched BIOS opcode

    // Reading from Unused memory regions

    // Accessing unused memory at 00004000h-01FFFFFFh, and 10000000h-FFFFFFFFh (and 02000000h-03FFFFFFh when RAM is 
    // disabled via Port 4000800h) returns the recently pre-fetched opcode.
}

const GBA_MemoryRegion* GBA_Memory::GetRegionFromType(GBA_MemoryRegionType type) const
{
    switch (type)
    {
        case GBA_MemoryRegionType::BIOS: return &biosRegion;
        case GBA_MemoryRegionType::EWRAM: return &ewramRegion;
        case GBA_MemoryRegionType::IWRAM: return &iwramRegion;
        case GBA_MemoryRegionType::IO: return &ioRegion;
        case GBA_MemoryRegionType::PaletteRAM: return &paletteRamRegion;
        case GBA_MemoryRegionType::VRAM: return &vramRegion;
        case GBA_MemoryRegionType::OAM: return &oamRegion;
        case GBA_MemoryRegionType::ROM0: return &rom0Region;
        case GBA_MemoryRegionType::ROM1: return &rom1Region;
        case GBA_MemoryRegionType::ROM2: return &rom2Region;
        case GBA_MemoryRegionType::SRAM: return &sramRegion;
        case GBA_MemoryRegionType::Invalid: return nullptr;

        default: return nullptr;
    }
}

const GBA_MemoryRegionType GBA_Memory::GetRegionTypeFromAddress(uint32_t address) const 
{
    switch (address >> 24)
    {
        case 0x00: return GBA_MemoryRegionType::BIOS;
        case 0x02: return GBA_MemoryRegionType::EWRAM;
        case 0x03: return GBA_MemoryRegionType::IWRAM;
        case 0x04: return GBA_MemoryRegionType::IO;
        case 0x05: return GBA_MemoryRegionType::PaletteRAM;
        case 0x06: return GBA_MemoryRegionType::VRAM;
        case 0x07: return GBA_MemoryRegionType::OAM;
        case 0x08: case 0x09: return GBA_MemoryRegionType::ROM0;
        case 0x0A: case 0x0B: return GBA_MemoryRegionType::ROM1;
        case 0x0C: case 0x0D: return GBA_MemoryRegionType::ROM2;
        case 0x0E: return GBA_MemoryRegionType::SRAM;

        default: return GBA_MemoryRegionType::Invalid;
    }
}

std::span<const uint8_t> GBA_Memory::GetRegionData(GBA_MemoryRegionType type) const 
{
    switch (type)
    {
        case GBA_MemoryRegionType::BIOS: return *bios;
        case GBA_MemoryRegionType::EWRAM: return *ewram;
        case GBA_MemoryRegionType::IWRAM: return *iwram;
        case GBA_MemoryRegionType::PaletteRAM: return *paletteRam;
        case GBA_MemoryRegionType::VRAM: return *vram;
        case GBA_MemoryRegionType::OAM: return *oam;
        case GBA_MemoryRegionType::ROM0: return *rom0;
        case GBA_MemoryRegionType::ROM1: return *rom1;
        case GBA_MemoryRegionType::ROM2: return *rom2;
        case GBA_MemoryRegionType::SRAM: return *sram;

        default: return {}; // Empty span
    }
}

std::span<uint8_t> GBA_Memory::GetRegionDataMutable(GBA_MemoryRegionType type) 
{
    switch (type)
    {
        case GBA_MemoryRegionType::BIOS: return *bios;
        case GBA_MemoryRegionType::EWRAM: return *ewram;
        case GBA_MemoryRegionType::IWRAM: return *iwram;
        case GBA_MemoryRegionType::PaletteRAM: return *paletteRam;
        case GBA_MemoryRegionType::VRAM: return *vram;
        case GBA_MemoryRegionType::OAM: return *oam;
        case GBA_MemoryRegionType::ROM0: return *rom0;
        case GBA_MemoryRegionType::ROM1: return *rom1;
        case GBA_MemoryRegionType::ROM2: return *rom2;
        case GBA_MemoryRegionType::SRAM: return *sram;

        default: return {}; // Empty span
    }
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
uint8_t GBA_Memory::Read8(uint32_t address, GBA_MemoryRegionType regionType)
{
    const GBA_MemoryRegion* region = GetRegionFromType(regionType);
    uint32_t offset = address - region->start;

    std::span<const uint8_t> regionData = GetRegionData(regionType);
    return regionData[offset];
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
uint16_t GBA_Memory::Read16(uint32_t address, GBA_MemoryRegionType regionType)
{
    const GBA_MemoryRegion* region = GetRegionFromType(regionType);
    uint32_t offset = address - region->start;

    std::span<const uint8_t> regionData = GetRegionData(regionType);
    uint16_t readValue = 
        static_cast<uint16_t>(regionData[offset]) | 
        static_cast<uint16_t>(regionData[offset + 1]) << 8;

    return readValue;
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
uint32_t GBA_Memory::Read32(uint32_t address, GBA_MemoryRegionType regionType)
{
    const GBA_MemoryRegion* region = GetRegionFromType(regionType);
    uint32_t offset = address - region->start;

    std::span<const uint8_t> regionData = GetRegionData(regionType);
    uint32_t readValue = 
        static_cast<uint16_t>(regionData[offset]) |
        static_cast<uint16_t>(regionData[offset + 1]) << 8  |
        static_cast<uint16_t>(regionData[offset + 2]) << 16 |
        static_cast<uint16_t>(regionData[offset + 3]) << 24;

    return readValue;
}   

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
void GBA_Memory::Write8(uint32_t address, uint8_t value, GBA_MemoryRegionType regionType)
{
    const GBA_MemoryRegion* region = GetRegionFromType(regionType);
    uint32_t offset = address - region->start;

    std::span<uint8_t> regionData = GetRegionDataMutable(regionType);
    regionData[offset] = value;
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
void GBA_Memory::Write16(uint32_t address, uint16_t value, GBA_MemoryRegionType regionType)
{
    const GBA_MemoryRegion* region = GetRegionFromType(regionType);
    uint32_t offset = address - region->start;

    std::span<uint8_t> regionData = GetRegionDataMutable(regionType);
    regionData[offset] = static_cast<uint8_t>(value & 0xFF);
    regionData[offset + 1] = static_cast<uint8_t>(value >> 8);
}

// Defensive checks are performed at the bus level, 
// the function assumes the access is valid at this point
void GBA_Memory::Write32(uint32_t address, uint32_t value, GBA_MemoryRegionType regionType)
{
    const GBA_MemoryRegion* region = GetRegionFromType(regionType);
    uint32_t offset = address - region->start;

    std::span<uint8_t> regionData = GetRegionDataMutable(regionType);
    regionData[offset] = static_cast<uint8_t>(value & 0xFF);
    regionData[offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    regionData[offset + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    regionData[offset + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

void GBA_Memory::LoadROM(const std::vector<uint8_t>& romData)
{
    std::copy(romData.begin(), romData.end(), rom0->begin());
    std::copy(romData.begin(), romData.end(), rom1->begin());
    std::copy(romData.begin(), romData.end(), rom2->begin());

    rom.PrintROMInfo();
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

void GBA_Memory::ClearRegion(GBA_MemoryRegionType type)
{
    std::span<uint8_t> regionData = GetRegionDataMutable(type); 
    std::fill(regionData.begin(), regionData.end(), 0);
}

void GBA_Memory::Clear8(uint32_t address)
{
    GBA_MemoryRegionType region = GetRegionTypeFromAddress(address);
    Write8(address, 0, region);
}

void GBA_Memory::Clear16(uint32_t address)
{
    GBA_MemoryRegionType region = GetRegionTypeFromAddress(address);
    Write16(address, 0, region);
}

void GBA_Memory::Clear32(uint32_t address)
{
    GBA_MemoryRegionType region = GetRegionTypeFromAddress(address);
    Write32(address, 0, region);
}

void GBA_Memory::ClearAddressRange(uint32_t startAddress, uint32_t endAddress)
{
    assert(startAddress <= endAddress && "Start address must be lower than the end address");

    const GBA_MemoryRegion* region = GetRegionFromAddress(startAddress);
    assert(region && "Invalid start address");
    assert(region == GetRegionFromAddress(endAddress) && "Start address and end address must pertain to the same region");

    // Ensure region is writeable (cannot clear BIOS nor ROM)
    if (region->writeMask == RNONE) return;
    if (region->type == GBA_MemoryRegionType::IO) return;

    std::span<uint8_t> regionData = GetRegionDataMutable(region->type); 

    uint32_t startOffset = startAddress - region->start;
    uint32_t endOffset = endAddress - region->start;

    assert(endOffset < regionData.size() && startOffset <= endOffset && "Attempting to clear more than one region");

    std::fill(regionData.begin() + startOffset, regionData.begin() + endOffset + 1, 0); 
}


