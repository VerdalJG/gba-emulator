#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <variant>
#include <memory>
#include <span>

#include "Core/GBA_Memory_Helpers.hpp"

#include "Utils/Logger.hpp"

class EmulatorCore;
class GBA_ROM;
class GBA_IO;

class GBA_Memory 
{
public:
    GBA_Memory() = delete;
    explicit GBA_Memory(EmulatorCore* core, GBA_ROM& rom, GBA_IO& io);
    ~GBA_Memory() = default;
    GBA_Memory(const GBA_Memory&) = delete; // Disable copy constructor
 
    const GBA_MemoryRegion* GetRegionFromAddress(uint32_t address) const;
    const GBA_MemoryRegion* GetRegionFromType(GBA_MemoryRegionType type) const;
    const GBA_MemoryRegionType GetRegionTypeFromAddress(uint32_t address) const;
    std::span<const uint8_t> GetRegionData(GBA_MemoryRegionType type) const;
    std::span<uint8_t> GetRegionDataMutable(GBA_MemoryRegionType type);
    
    MemReadResult<uint8_t> Read8(uint32_t address, GBA_MemoryRegionType regionType);
    MemReadResult<uint16_t> Read16(uint32_t address, GBA_MemoryRegionType regionType);
    MemReadResult<uint32_t> Read32(uint32_t address, GBA_MemoryRegionType regionType);

    void Write8(uint32_t address, uint8_t value, GBA_MemoryRegionType regionType);
    void Write16(uint32_t address, uint16_t value, GBA_MemoryRegionType regionType);
    void Write32(uint32_t address, uint32_t value, GBA_MemoryRegionType regionType);

    void ClearRegion(GBA_MemoryRegionType type);
    void Clear8(uint32_t address);
    void Clear16(uint32_t address);
    void Clear32(uint32_t address);
    void ClearAddressRange(uint32_t startAddress, uint32_t endAddress);

    void LoadROM(const std::vector<uint8_t>& romData);
    void LoadBIOS(const std::vector<uint8_t>& biosData);
    
private:
    std::unique_ptr<std::vector<uint8_t>> bios;
    std::unique_ptr<std::vector<uint8_t>> ewram;
    std::unique_ptr<std::vector<uint8_t>> iwram;
    std::unique_ptr<std::vector<uint8_t>> paletteRam;
    std::unique_ptr<std::vector<uint8_t>> vram;
    std::unique_ptr<std::vector<uint8_t>> oam;
    std::unique_ptr<std::vector<uint8_t>> rom0;
    std::unique_ptr<std::vector<uint8_t>> rom1;
    std::unique_ptr<std::vector<uint8_t>> rom2;
    std::unique_ptr<std::vector<uint8_t>> sram;
    
    // General internal memory
    GBA_MemoryRegion biosRegion = GBA_MemoryRegion(BIOS_START, BIOS_END, BusAccessSize::Word, RALL, RNONE, GBA_MemoryRegionType::BIOS);
    GBA_MemoryRegion ewramRegion = GBA_MemoryRegion(EWRAM_START, EWRAM_END, BusAccessSize::Halfword, RALL, RALL, GBA_MemoryRegionType::EWRAM); // External work RAM
    GBA_MemoryRegion iwramRegion = GBA_MemoryRegion(IWRAM_START, IWRAM_END, BusAccessSize::Word, RALL, RALL, GBA_MemoryRegionType::IWRAM); // Internal work RAM
    GBA_MemoryRegion ioRegion = GBA_MemoryRegion(IO_START, IO_END, BusAccessSize::Word, RALL, RALL, GBA_MemoryRegionType::IO);

    // Internal display memory (+ 1 cycle if GBA accesses video memory at the same time)
    GBA_MemoryRegion paletteRamRegion = GBA_MemoryRegion(PALETTE_RAM_START, PALETTE_RAM_END, BusAccessSize::Halfword, RALL, R16_32, GBA_MemoryRegionType::PaletteRAM);
    GBA_MemoryRegion vramRegion = GBA_MemoryRegion(VRAM_START, VRAM_END, BusAccessSize::Halfword, RALL, R16_32, GBA_MemoryRegionType::VRAM); // Video RAM
    GBA_MemoryRegion oamRegion = GBA_MemoryRegion(OAM_START, OAM_END, BusAccessSize::Word, RALL, R16_32, GBA_MemoryRegionType::OAM) ; // Object-Attribute RAM
    
    // External memory (cartridge)

    // ROM0/ROM1/ROM2 all point to the same ROM data but differ by waitstate timing.
    // ROM1 and ROM2 are mirrors of ROM0 at different addresses (for access timing differences).

    GBA_MemoryRegion rom0Region = GBA_MemoryRegion(ROM0_START, ROM0_END, BusAccessSize::Halfword, RALL, RNONE, GBA_MemoryRegionType::ROM0);
    GBA_MemoryRegion rom1Region = GBA_MemoryRegion(ROM1_START, ROM1_END, BusAccessSize::Halfword, RALL, RNONE, GBA_MemoryRegionType::ROM1);
    GBA_MemoryRegion rom2Region = GBA_MemoryRegion(ROM2_START, ROM2_END, BusAccessSize::Halfword, RALL, RNONE, GBA_MemoryRegionType::ROM2);
    GBA_MemoryRegion sramRegion = GBA_MemoryRegion(SRAM_START, SRAM_END, BusAccessSize::Byte, R8, R8, GBA_MemoryRegionType::SRAM);

    // All memory (except GamePak SRAM) can be accessed by 16bit and 32bit DMA.

    // The GBA has unused memory area after the SRAM, which goes from 0x10000000 to 0xFFFFFFFF

    EmulatorCore* core;
    GBA_ROM& rom;
    GBA_IO& io;
};

// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE