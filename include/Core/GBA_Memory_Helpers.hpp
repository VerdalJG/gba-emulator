#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <cmath>

#include "Core/GBA_WaitstateController.hpp"
#include "Core/GBA_Bus.hpp"

// Memory map locations for GBA (inclusive ranges)
constexpr uint32_t BIOS_START = 0x00000000;
constexpr uint32_t BIOS_END = 0x00003FFF;

constexpr uint32_t EWRAM_START = 0x02000000;
constexpr uint32_t EWRAM_END = 0x0203FFFF;

constexpr uint32_t IWRAM_START = 0x03000000;
constexpr uint32_t IWRAM_END = 0x03007FFF;

constexpr uint32_t IO_START = 0x04000000;
constexpr uint32_t IO_END = 0x040003FF;

constexpr uint32_t PALETTE_RAM_START = 0x05000000;
constexpr uint32_t PALETTE_RAM_END = 0x050003FF;

constexpr uint32_t VRAM_START = 0x06000000;
constexpr uint32_t VRAM_END = 0x06017FFF;

constexpr uint32_t OAM_START = 0x07000000;
constexpr uint32_t OAM_END = 0x070003FF;

constexpr uint32_t ROM0_START = 0x08000000;
constexpr uint32_t ROM0_END = 0x09FFFFFF;

constexpr uint32_t ROM1_START = 0x0A000000;
constexpr uint32_t ROM1_END = 0x0BFFFFFF;

constexpr uint32_t ROM2_START = 0x0C000000;
constexpr uint32_t ROM2_END = 0x0DFFFFFF;

constexpr uint32_t SRAM_START = 0x0E000000;
constexpr uint32_t SRAM_END = 0x0E00FFFF;

// Memory map sizes for GBA
constexpr size_t BIOS_SIZE = 0x4000; // 16KB
constexpr size_t EWRAM_SIZE = 0x40000; // External work RAM / On GBA mainboard RAM - 256KB
constexpr size_t IWRAM_SIZE = 0x8000; // Internal work RAM / On CPU chip RAM - 32KB - Fastest RAM
constexpr size_t IO_SIZE = 0x3FF; // I/O Registers - 1024 bytes - documentation is specifically 0x3FF
constexpr size_t PALETTE_RAM_SIZE = 0x400; // Palette RAM - 1KB
constexpr size_t VRAM_SIZE = 0x18000; // Video RAM - 144KB
constexpr size_t OAM_SIZE = 0x400; // Object Attribute Memory - 1KB
constexpr size_t ROM_BANK_SIZE = 0x2000000; // Maximum ROM size - 32MB, most games are less than 16MB
constexpr size_t SRAM_SIZE = 0x10000; // 64 KB

enum class AccessType
{
   Read,
   Write
};

enum class GBA_MemoryRegionType
{
    BIOS,
    EWRAM,
    IWRAM,
    IO,
    PaletteRAM,
    VRAM,
    OAM,
    ROM0,
    ROM1,
    ROM2,
    SRAM,
    Invalid
};

// Access masks
constexpr uint8_t R8 = static_cast<uint8_t>(BusAccessSize::Byte);
constexpr uint8_t R16 = static_cast<uint8_t>(BusAccessSize::Halfword);
constexpr uint8_t R32 = static_cast<uint8_t>(BusAccessSize::Word);
constexpr uint8_t R8_16 = R8 | R16;
constexpr uint8_t R16_32 = R16 | R32;
constexpr uint8_t RALL = R8 | R16 | R32;
constexpr uint8_t RNONE = 0;

struct GBA_MemoryRegion
{
    uint32_t start;
    uint32_t end;
    BusAccessSize busWidth;

    // Bitmask for R/W permissions 8/16/32
    uint8_t readMask : 3;
    uint8_t writeMask : 3; 

    GBA_MemoryRegionType type;

    GBA_MemoryRegion(uint32_t start, uint32_t end, BusAccessSize busWidth, 
        uint8_t readMask, uint8_t writeMask, GBA_MemoryRegionType type) :
        start(start), end(end), busWidth(busWidth), readMask(readMask), writeMask(writeMask), type(type)
    {}

    bool IsValidAccess(uint32_t address, AccessType access, BusAccessSize accessSize) const
    {
        size_t sizeBytes = static_cast<size_t>(accessSize);
        bool withinBounds = (address >= start) && ((address + sizeBytes) <= end);

        bool isRead = access == AccessType::Read;
        bool allowed = isRead ? (readMask & sizeBytes) : (writeMask & sizeBytes);

        return withinBounds && allowed;
    }

    int AccessesRequired(BusAccessSize size) const
    {
        uint32_t access = static_cast<uint32_t>(size);
        uint32_t bus = static_cast<uint32_t>(busWidth);

        return (access + bus - 1) / bus;
    }
};
