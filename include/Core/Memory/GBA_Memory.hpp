#pragma once
#include "Core/Memory/GBA_Memory_Helpers.hpp"

#include "Utils/Logger.hpp"

#include <cstdint>
#include <vector>
#include <array>
#include <variant>
#include <memory>
#include <span>
#include "Utils/Logger.hpp"

class EmulatorCore;
class GBA_ROM;
class GBA_IO;

class GBA_Memory 
{
public:
    GBA_Memory() = delete;
    explicit GBA_Memory(EmulatorCore* core, GBA_ROM& rom);
    ~GBA_Memory() = default;
    GBA_Memory(const GBA_Memory&) = delete; // Disable copy constructor
 
    const MemoryRegion* GetRegionFromAddress(uint32_t address) const;
    const MemoryRegion* GetRegionFromType(RegionType type) const;
    std::span<const uint8_t> GetRegionData(RegionType type) const;
    std::span<uint8_t> GetRegionDataMutable(RegionType type);
    uint32_t ComputeAccessOffset(uint32_t address, RegionType regionType);
    
    MemReadResult<uint8_t> Read8(uint32_t address, RegionType regionType);
    MemReadResult<uint16_t> Read16(uint32_t address, RegionType regionType);
    MemReadResult<uint32_t> Read32(uint32_t address, RegionType regionType);

    void Write8(uint32_t address, uint8_t value, RegionType regionType);
    void Write16(uint32_t address, uint16_t value, RegionType regionType);
    void Write32(uint32_t address, uint32_t value, RegionType regionType);

    void ClearRegion(RegionType type);
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
    MemoryRegion biosRegion = MemoryRegion(BIOS_START, BIOS_END, BIOS_SIZE, AccessSize::Word, RegionType::BIOS, Mirroring::NoMirror);
    MemoryRegion ewramRegion = MemoryRegion(EWRAM_START, EWRAM_END, EWRAM_SIZE, AccessSize::Halfword, RegionType::EWRAM, Mirroring::Mirror); // External work RAM
    MemoryRegion iwramRegion = MemoryRegion(IWRAM_START, IWRAM_END, IWRAM_SIZE, AccessSize::Word, RegionType::IWRAM, Mirroring::Mirror); // Internal work RAM
    MemoryRegion ioRegion = MemoryRegion(IO_START, IO_END, IO_SIZE, AccessSize::Word, RegionType::IO, Mirroring::SpecialMirror);

    // Internal display memory (+ 1 cycle if GBA accesses video memory at the same time)
    MemoryRegion paletteRamRegion = MemoryRegion(PALETTE_RAM_START, PALETTE_RAM_END, PALETTE_RAM_SIZE, AccessSize::Halfword, RegionType::PaletteRAM, Mirroring::Mirror);
    MemoryRegion vramRegion = MemoryRegion(VRAM_START, VRAM_END, VRAM_TOTAL_SIZE, AccessSize::Halfword, RegionType::VRAM, Mirroring::SpecialMirror); // Video RAM
    MemoryRegion oamRegion = MemoryRegion(OAM_START, OAM_END, OAM_SIZE, AccessSize::Word, RegionType::OAM, Mirroring::Mirror); // Object-Attribute RAM
    
    // External memory (cartridge)

    // ROM0/ROM1/ROM2 all point to the same ROM data but differ by waitstate timing.
    // ROM1 and ROM2 are mirrors of ROM0 at different addresses (for access timing differences).

    MemoryRegion rom0Region = MemoryRegion(ROM0_START, ROM0_END, ROM_BANK_SIZE, AccessSize::Halfword, RegionType::ROM0, Mirroring::NoMirror);
    MemoryRegion rom1Region = MemoryRegion(ROM1_START, ROM1_END, ROM_BANK_SIZE, AccessSize::Halfword, RegionType::ROM1, Mirroring::NoMirror);
    MemoryRegion rom2Region = MemoryRegion(ROM2_START, ROM2_END, ROM_BANK_SIZE, AccessSize::Halfword, RegionType::ROM2, Mirroring::NoMirror);
    MemoryRegion sramRegion = MemoryRegion(SRAM_START, SRAM_END, SRAM_SIZE, AccessSize::Byte, RegionType::SRAM, Mirroring::NoMirror);

    // All memory (except GamePak SRAM) can be accessed by 16bit and 32bit DMA.
    // The GBA has unused memory area after the SRAM, which goes from 0x10000000 to 0xFFFFFFFF

    EmulatorCore* core;
    GBA_ROM& rom;
};

// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE