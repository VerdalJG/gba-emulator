#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <cmath>

#include "Core/GBA_WaitstateController.hpp"

// Memory map locations for GBA (inclusive ranges)
constexpr uint32_t BIOS_START = 0x00000000;
constexpr uint32_t BIOS_END = 0x00003FFF;

constexpr uint32_t EWRAM_START = 0x02000000;
constexpr uint32_t EWRAM_END = 0x0203FFFF;

constexpr uint32_t IWRAM_START = 0x03000000;
constexpr uint32_t IWRAM_END = 0x03007FFF;

constexpr uint32_t IOREGISTERS_START = 0x04000000;
constexpr uint32_t IOREGISTERS_END = 0x040003FF;

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
constexpr size_t IOREGISTERS_SIZE = 0x3FF; // I/O Registers - 1024 bytes - documentation is specifically 0x3FF
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

enum class RegionType
{
    BIOS,
    EWRAM,
    IWRAM,
    IORegisters,
    PaletteRAM,
    VRAM,
    OAM,
    ROM0,
    ROM1,
    ROM2,
    SRAM,
    Invalid
};

enum class AccessSize
{
    Byte = 1,
    Halfword = 2,
    Word = 4
};

struct LastBusAccess
{
    uint32_t value = 0;
    AccessSize size = AccessSize::Word;
};

// Access masks
constexpr uint8_t R8 = static_cast<uint8_t>(AccessSize::Byte);
constexpr uint8_t R16 = static_cast<uint8_t>(AccessSize::Halfword);
constexpr uint8_t R32 = static_cast<uint8_t>(AccessSize::Word);
constexpr uint8_t R8_16 = R8 | R16;
constexpr uint8_t R16_32 = R16 | R32;
constexpr uint8_t RALL = R8 | R16 | R32;
constexpr uint8_t RNONE = 0;

struct MemoryRegion
{
    std::shared_ptr<std::vector<uint8_t>> data;
    uint32_t start;
    uint32_t end;
    AccessSize busWidth;

    // Bitmask for R/W permissions 8/16/32
    uint8_t readMask : 3;
    uint8_t writeMask : 3; 

    RegionType type;

    MemoryRegion(uint32_t start, uint32_t end, size_t size, AccessSize busWidth, 
        uint8_t readMask, uint8_t writeMask, RegionType type) :
        data(std::make_shared<std::vector<uint8_t>>(size)), start(start), end(end), 
        busWidth(busWidth), readMask(readMask), writeMask(writeMask), type(type)
    {}

    bool IsValidAccess(uint32_t address, AccessType access, AccessSize size)
    {
        bool withinBounds = address >= start && address <= end;

        bool isRead = access == AccessType::Read;
        size_t sizeBytes = static_cast<size_t>(size);

        bool allowed = isRead ? (readMask & sizeBytes) : (writeMask & sizeBytes);

        return withinBounds && allowed;
    }

    int AccessesRequired(AccessSize size)
    {
        uint32_t access = static_cast<uint32_t>(size);
        uint32_t bus = static_cast<uint32_t>(busWidth);

        return (access + bus - 1) / bus;
    }

    // Factory functions for each GBA region
    static MemoryRegion BIOS()
    {
        return MemoryRegion(
            BIOS_START, 
            BIOS_END, 
            BIOS_SIZE, 
            AccessSize::Word, 
            RALL, 
            RNONE, 
            RegionType::BIOS
        );
    }

    static MemoryRegion EWRAM()
    {
        return MemoryRegion(
            EWRAM_START, 
            EWRAM_END, 
            EWRAM_SIZE, 
            AccessSize::Word, 
            RALL, 
            RALL, 
            RegionType::EWRAM
        );
    }

    static MemoryRegion IWRAM()
    {
        return MemoryRegion(
            IWRAM_START, 
            IWRAM_END, 
            IWRAM_SIZE, 
            AccessSize::Word, 
            RALL, 
            RALL, 
            RegionType::IWRAM
        );
    }

    static MemoryRegion IORegisters()
    {
        return MemoryRegion(
            IOREGISTERS_START, 
            IOREGISTERS_END, 
            IOREGISTERS_SIZE, 
            AccessSize::Word, 
            RALL, 
            RALL, 
            RegionType::IORegisters
        );
    }

    static MemoryRegion Palette_RAM()
    {
        return MemoryRegion(
            PALETTE_RAM_START, 
            PALETTE_RAM_END, 
            PALETTE_RAM_SIZE, 
            AccessSize::Halfword, 
            RALL, 
            R16_32, 
            RegionType::PaletteRAM
        );
    }

    static MemoryRegion VRAM()
    {
        return MemoryRegion(
            VRAM_START, 
            VRAM_END, 
            VRAM_SIZE, 
            AccessSize::Halfword, 
            RALL, 
            R16_32, 
            RegionType::VRAM
        );
    }

    static MemoryRegion OAM()
    {
        return MemoryRegion(
            OAM_START, 
            OAM_END, 
            OAM_SIZE, 
            AccessSize::Halfword, 
            RALL, 
            R16_32, 
            RegionType::OAM
        );
    }

    static MemoryRegion ROM0()
    {
        return MemoryRegion(
            ROM0_START, 
            ROM0_END, 
            ROM_BANK_SIZE, 
            AccessSize::Halfword, 
            RALL, 
            RNONE, 
            RegionType::ROM0
        );
    }

    static MemoryRegion ROM1()
    {
        return MemoryRegion(
            ROM1_START, 
            ROM1_END, 
            ROM_BANK_SIZE, 
            AccessSize::Halfword, 
            RALL, 
            RNONE, 
            RegionType::ROM1
        );
    }

    static MemoryRegion ROM2()
    {
        return MemoryRegion(
            ROM2_START, 
            ROM2_END, 
            ROM_BANK_SIZE, 
            AccessSize::Halfword, 
            RALL, 
            RNONE, 
            RegionType::ROM2
        );
    }

    static MemoryRegion SRAM()
    {
        return MemoryRegion(
            SRAM_START, 
            SRAM_END, 
            SRAM_SIZE, 
            AccessSize::Byte, 
            R8, 
            R8, 
            RegionType::SRAM
        );
    }
};


