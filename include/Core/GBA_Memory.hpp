#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <variant>
#include <memory>

#include "Core/GBA_ROM.hpp"
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
    SRAM
};

struct MemoryRegion
{
    std::shared_ptr<std::vector<uint8_t>> data;
    Permissions permissions;
    uint32_t startAddress;

    MemoryRegion(Permissions permissions, uint32_t startAddress, size_t regionSize) 
    :   permissions(permissions), startAddress(startAddress),
        data(std::make_shared<std::vector<uint8_t>>(regionSize)) {}

    MemoryRegion(Permissions permissions, uint32_t startAddress)
    :   permissions(permissions), startAddress(startAddress), data(nullptr) {}
};

class EmulatorCore;

class GBA_Memory 
{
public:
    GBA_Memory() = delete;
    explicit GBA_Memory(EmulatorCore* core);
    ~GBA_Memory() = default;
    GBA_Memory(const GBA_Memory&) = delete; // Disable copy constructor
 
    
    MemoryRegion* GetRegionFromAddress(uint32_t address);
    MemoryRegion* GetRegionFromType(RegionType type);
    
    uint8_t Read8(uint32_t address);
    uint16_t Read16(uint32_t address);
    uint32_t Read32(uint32_t address);

    void Write8(uint32_t address, uint8_t value);
    void Write16(uint32_t address, uint32_t value);
    void Write32(uint32_t address, uint32_t value);

    void LoadROM(const std::vector<uint8_t>& romData);
    void LoadBIOS(const std::vector<uint8_t>& biosData);
    void ClearRegion(RegionType type);
    void Clear8(uint32_t address);
    void Clear16(uint32_t address);
    void Clear32(uint32_t address);
    void ClearAddressRange(uint32_t startAddress, uint32_t endAddress);


    void ResetSIORegisters();
    void ResetSoundRegisters();
    void ResetOtherIORegisters();

    
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

    MemoryRegion rom0 = MemoryRegion(Permissions::ReadOnly, ROM0_START);
    MemoryRegion rom1 = MemoryRegion(Permissions::ReadOnly, ROM1_START);
    MemoryRegion rom2 = MemoryRegion(Permissions::ReadOnly, ROM2_START);
    MemoryRegion sram = MemoryRegion(Permissions::ReadWrite, SRAM_START, SRAM_SIZE);

    // The GBA has unused memory area after the SRAM, which goes from 0x10000000 to 0xFFFFFFFF

    uint8_t lastBusValue = 0xFF; // For open-bus emulation

    //std::shared_ptr<std::vector<uint8_t>> rom;
    std::unique_ptr<GBA_ROM> rom;
    EmulatorCore* core; 
    GBA_WaitstateController waitstateController;

    template <typename T>
    T Read(uint32_t address, AccessSize size)
    {
        const uint32_t alignmentMask =  (sizeof(T) == 1) ? 0u :
                                        (sizeof(T) == 2) ? 1u :
                                        (sizeof(T) == 4) ? 3u;

        address &= alignmentMask;

        MemoryRegion* region = GetRegionFromAddress(address);

        if (!region || region->data)
        {
            T busFill = 0;
        }
    }

};


// TODO: Functions that interact with memory - memory init, load rom, read and write


// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE