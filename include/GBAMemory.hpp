#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <variant>

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

struct MemoryRegion
{
    std::vector<uint8_t>* data;
    Permissions permissions;
    uint32_t startAddress;

    MemoryRegion(Permissions permissions, uint32_t startAddress, size_t regionSize)
    {
        this->permissions = permissions;
        this->startAddress = startAddress;
        data = new std::vector<uint8_t>(regionSize);
    }

    ~MemoryRegion()
    {
        delete data;
    }
}; 

class GBAMemory 
{
public:
    GBAMemory() = default;
    ~GBAMemory() = default;
    GBAMemory(const GBAMemory&) = delete; // Disable copy constructor

    
    MemoryRegion* GetRegionFromAddress(uint32_t address);
    //void WriteToRegion(std::vector<uint8_t> data, MemoryRegion region);
    uint8_t Read8(uint32_t address);
    uint16_t Read16(uint32_t address);
    uint32_t Read32(uint32_t address);

    void Write8(uint32_t address, uint8_t value);
    void Write16(uint32_t address, uint16_t value);
    void Write32(uint32_t address, uint32_t value);

    void LoadROM(const std::vector<uint8_t>& romData);
    void LoadBIOS(const std::vector<uint8_t>& biosData);

    
private:
    // General internal memory
    MemoryRegion bios = MemoryRegion(Permissions::ReadOnly, BIOS_START, BIOS_SIZE);
    MemoryRegion ewram = MemoryRegion(Permissions::ReadWrite, EWRAM_START, EWRAM_SIZE); // External work RAM
    MemoryRegion iwram = MemoryRegion(Permissions::ReadWrite, IWRAM_START, IWRAM_SIZE); // Internal work RAM
    MemoryRegion ioRegisters = MemoryRegion(Permissions::Varies, IOREGISTERS_START, IOREGISTERS_SIZE);

    // Internal display memory
    MemoryRegion paletteRam = MemoryRegion(Permissions::ReadWrite, PALETTE_RAM_START, PALETTE_RAM_SIZE);
    MemoryRegion vram = MemoryRegion(Permissions::ReadWrite, VRAM_START, VRAM_SIZE); // Video RAM
    MemoryRegion oam = MemoryRegion(Permissions::ReadWrite, OAM_START, OAM_SIZE);
    
    // External memory (cartridge)

    // ROM0/1/2 all point to the same ROM data but differ by waitstate timing.
    // ROM1 and ROM2 are mirrors of ROM0 at different addresses (for access timing differences).

    MemoryRegion rom0 = MemoryRegion(Permissions::ReadOnly, ROM0_START, ROM_BANK_SIZE);
    MemoryRegion rom1 = MemoryRegion(Permissions::ReadOnly, ROM1_START, ROM_BANK_SIZE);
    MemoryRegion rom2 = MemoryRegion(Permissions::ReadOnly, ROM2_START, ROM_BANK_SIZE);
    MemoryRegion sram = MemoryRegion(Permissions::ReadWrite, SRAM_START, SRAM_SIZE);

    // The GBA has unused memory area after the SRAM, which goes from 0x10000000 to 0xFFFFFFFF

    uint8_t lastBusValue = 0xFF; // For open-bus emulation

    std::vector<uint8_t> rom; 

};


// TODO: Functions that interact with memory - memory init, load rom, read and write


// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE