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
    uint32_t ComputeAccessOffset(uint32_t address, GBA_MemoryRegionType regionType);
    
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

    void InitROMBanks();
    void LoadBIOS(const std::vector<uint8_t>& biosData);
    
private:
    std::unique_ptr<std::vector<uint8_t>> bios;
    std::unique_ptr<std::vector<uint8_t>> ewram;
    std::unique_ptr<std::vector<uint8_t>> iwram;
    std::unique_ptr<std::vector<uint8_t>> paletteRam;
    std::unique_ptr<std::vector<uint8_t>> vram;
    std::unique_ptr<std::vector<uint8_t>> oam;
    std::span<const uint8_t> rom0View;
    std::span<const uint8_t> rom1View;
    std::span<const uint8_t> rom2View;
    std::unique_ptr<std::vector<uint8_t>> sram;
    
    // General internal memory
    GBA_MemoryRegion biosRegion = GBA_MemoryRegion(BIOS_START, BIOS_END, BIOS_SIZE, BusAccessSize::Word, RALL, RNONE, GBA_MemoryRegionType::BIOS, Mirroring::NoMirror);
    GBA_MemoryRegion ewramRegion = GBA_MemoryRegion(EWRAM_START, EWRAM_END, EWRAM_SIZE, BusAccessSize::Halfword, RALL, RALL, GBA_MemoryRegionType::EWRAM, Mirroring::Mirror); // External work RAM
    GBA_MemoryRegion iwramRegion = GBA_MemoryRegion(IWRAM_START, IWRAM_END, IWRAM_SIZE, BusAccessSize::Word, RALL, RALL, GBA_MemoryRegionType::IWRAM, Mirroring::Mirror); // Internal work RAM
    GBA_MemoryRegion ioRegion = GBA_MemoryRegion(IO_START, IO_END, IO_SIZE, BusAccessSize::Word, RALL, RALL, GBA_MemoryRegionType::IO, Mirroring::SpecialMirror);

    // Internal display memory (+ 1 cycle if GBA accesses video memory at the same time)
    GBA_MemoryRegion paletteRamRegion = GBA_MemoryRegion(PALETTE_RAM_START, PALETTE_RAM_END, PALETTE_RAM_SIZE, BusAccessSize::Halfword, RALL, R16_32, GBA_MemoryRegionType::PaletteRAM, Mirroring::Mirror);
    GBA_MemoryRegion vramRegion = GBA_MemoryRegion(VRAM_START, VRAM_END, VRAM_TOTAL_SIZE, BusAccessSize::Halfword, RALL, R16_32, GBA_MemoryRegionType::VRAM, Mirroring::SpecialMirror); // Video RAM
    GBA_MemoryRegion oamRegion = GBA_MemoryRegion(OAM_START, OAM_END, OAM_SIZE, BusAccessSize::Word, RALL, R16_32, GBA_MemoryRegionType::OAM, Mirroring::Mirror); // Object-Attribute RAM
    
    // External memory (cartridge)

    // ROM0/ROM1/ROM2 all point to the same ROM data but differ by waitstate timing.
    // ROM1 and ROM2 are mirrors of ROM0 at different addresses (for access timing differences).

    GBA_MemoryRegion rom0Region = GBA_MemoryRegion(ROM0_START, ROM0_END, ROM_BANK_SIZE, BusAccessSize::Halfword, RALL, RNONE, GBA_MemoryRegionType::ROM0, Mirroring::NoMirror);
    GBA_MemoryRegion rom1Region = GBA_MemoryRegion(ROM1_START, ROM1_END, ROM_BANK_SIZE, BusAccessSize::Halfword, RALL, RNONE, GBA_MemoryRegionType::ROM1, Mirroring::NoMirror);
    GBA_MemoryRegion rom2Region = GBA_MemoryRegion(ROM2_START, ROM2_END, ROM_BANK_SIZE, BusAccessSize::Halfword, RALL, RNONE, GBA_MemoryRegionType::ROM2, Mirroring::NoMirror);
    GBA_MemoryRegion sramRegion = GBA_MemoryRegion(SRAM_START, SRAM_END, SRAM_SIZE, BusAccessSize::Byte, R8, R8, GBA_MemoryRegionType::SRAM, Mirroring::SpecialMirror);

    // All memory (except GamePak SRAM) can be accessed by 16bit and 32bit DMA.
    // The GBA has unused memory area after the SRAM, which goes from 0x10000000 to 0xFFFFFFFF

    EmulatorCore* core;
    GBA_ROM& rom;
    GBA_IO& io;
};

// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE