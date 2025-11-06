#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "Core/GBA_WaitstateController.hpp"

// Memory map locations for GBA
constexpr uint32_t BIOS_START = 0X00000000;
constexpr uint32_t EWRAM_START = 0x02000000;
constexpr uint32_t IWRAM_START = 0x03000000;
constexpr uint32_t IOREGISTERS_START = 0x04000000;
constexpr uint32_t PALETTE_RAM_START = 0x05000000;
constexpr uint32_t VRAM_START = 0x06000000;
constexpr uint32_t OAM_START = 0x07000000;
constexpr uint32_t ROM0_START = 0x08000000;
constexpr uint32_t ROM1_START = 0x0A000000;
constexpr uint32_t ROM2_START = 0x0C000000;
constexpr uint32_t SRAM_START = 0x0E000000;

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

enum class Permissions 
{
    ReadOnly,
    ReadWrite,
    Varies
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

struct MemoryRegion
{
    std::shared_ptr<std::vector<uint8_t>> data;
    Permissions permissions;
    uint32_t startAddress;
    RegionType type;

    MemoryRegion(Permissions permissions, uint32_t startAddress, size_t regionSize, RegionType type) 
    :   permissions(permissions), startAddress(startAddress),
        data(std::make_shared<std::vector<uint8_t>>(regionSize)),
        type(type) {}

    MemoryRegion(Permissions permissions, uint32_t startAddress, RegionType type)
    :   permissions(permissions), startAddress(startAddress), data(nullptr),
        type(type) {}
};

struct LastBusAccess
{
    uint32_t address = 0;
    AccessSize size = AccessSize::Word;
    RegionType region = RegionType::Invalid;

    void UpdateValues(uint32_t address, size_t size, const MemoryRegion* region)
    {
        this->address = address;
        this->size = static_cast<AccessSize>(size);
        this->region = region->type;
    }
};