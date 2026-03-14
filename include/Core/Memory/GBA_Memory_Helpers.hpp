#pragma once
#include "Utils/Integers.hpp"

#include <memory>
#include <vector>
#include <cmath>

// Memory map locations for GBA (inclusive ranges)
constexpr u32 BIOS_START = 0x00000000;
constexpr u32 BIOS_END = 0x00003FFF;

constexpr u32 EWRAM_START = 0x02000000;
constexpr u32 EWRAM_END = 0x02FFFFFF;

constexpr u32 IWRAM_START = 0x03000000;
constexpr u32 IWRAM_END = 0x03FFFFFF;

constexpr u32 IO_START = 0x04000000;
constexpr u32 IO_END = 0x04FFFFFF;

constexpr u32 PALETTE_RAM_START = 0x05000000;
constexpr u32 PALETTE_RAM_END = 0x05FFFFFF;

constexpr u32 VRAM_START = 0x06000000;
constexpr u32 VRAM_END = 0x06FFFFFF;

constexpr u32 VRAM_BOUNDARY_BITMAP_OFFSET = 0x4000; 

constexpr u32 OAM_START = 0x07000000;
constexpr u32 OAM_END = 0x07FFFFFF;

constexpr u32 ROM0_START = 0x08000000;
constexpr u32 ROM0_END = 0x09FFFFFF;

constexpr u32 ROM1_START = 0x0A000000;
constexpr u32 ROM1_END = 0x0BFFFFFF;

constexpr u32 ROM2_START = 0x0C000000;
constexpr u32 ROM2_END = 0x0DFFFFFF;

constexpr u32 SRAM_START = 0x0E000000;
constexpr u32 SRAM_END = 0x0FFFFFFF;

// Memory map sizes for GBA
constexpr size_t BIOS_SIZE = 0x4000; // 16KB
constexpr size_t EWRAM_SIZE = 0x40000; // External work RAM / On GBA mainboard RAM - 256KB
constexpr size_t IWRAM_SIZE = 0x8000; // Internal work RAM / On CPU chip RAM - 32KB - Fastest RAM
constexpr size_t IO_SIZE = 0x3FF; // I/O Registers - 1024 bytes - documentation is specifically 0x3FF
constexpr size_t PALETTE_RAM_SIZE = 0x400; // Palette RAM - 1KB
constexpr size_t VRAM_BG_SIZE = 0x10000; // 64KB
constexpr size_t VRAM_OBJ_SIZE = 0x8000; // 32KB
constexpr size_t VRAM_TOTAL_SIZE = VRAM_BG_SIZE + VRAM_OBJ_SIZE; // Video RAM - 96KB
constexpr size_t VRAM_MIRROR_SIZE = VRAM_BG_SIZE + VRAM_OBJ_SIZE + VRAM_OBJ_SIZE; // Mirror window - 128 KB
constexpr size_t OAM_SIZE = 0x400; // Object Attribute Memory - 1KB
constexpr size_t ROM_BANK_SIZE = 0x2000000; // Maximum ROM size - 32MB, most games are less than 16MB
constexpr size_t SRAM_SIZE = 0x10000; // 64 KB
constexpr size_t SRAM_MIRROR_SIZE = 0x8000; // 32 KB

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

enum AccessSize
{
    Byte = 1,
    Halfword = 2,
    Word = 4,
    Invalid = 0
};

enum class BusDomain
{
    GamePakROM,
    Other
};

enum class Mirroring
{
    Mirror,
    NoMirror,
    SpecialMirror
};

template<typename T>
struct MemReadResult 
{
    T value;
    bool valid;
};

#define OPEN_BUS {0, false}

struct MemoryRegion
{
    u32 start;
    u32 end;
    u32 physicalSize;
    AccessSize busWidth;
    Mirroring mirroring;
    RegionType type;

    MemoryRegion(u32 start, u32 end, u32 size, AccessSize busWidth, RegionType type, Mirroring mirroring) :
        start(start), end(end), physicalSize(size), busWidth(busWidth), type(type), mirroring(mirroring)
    {}

    template <typename T>
    bool IsValidAccess(u32 address) const
    {
        AccessSize accessSize = sizeof(T);
        bool withinBounds = (address >= start) && ((address + (accessSize - 1)) <= end);
        return withinBounds;
    }
   
    bool IsVideoMemory() const
    {       
        return type == RegionType::PaletteRAM || type == RegionType::VRAM || type == RegionType::OAM;
    }
    
    bool IsROMRegion() const
    {
        return type == RegionType::ROM0 || type == RegionType::ROM1 || type == RegionType::ROM2;
    }
};

/*
VRAM, OAM, and Palette RAM Access
- These memory regions can be accessed during H-Blank or V-Blank only 
(unless display is disabled by Forced Blank bit in DISPCNT register).

- There is an additional restriction for OAM memory: Accesses during H-Blank are allowed only if 
'H-Blank Interval Free' in DISPCNT is set (which'd reduce number of display-able OBJs though).

- The CPU appears to be able to access VRAM/OAM/Palette at any time, a waitstate (one clock cycle) 
being inserted automatically in case that the display controller was accessing memory simultaneously. 
(Ie. unlike as in old 8bit gameboy, the data will not get lost.)
*/
