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
 
    const MemoryRegion* GetRegionFromAddress(u32 address) const;
    const MemoryRegion* GetRegionFromType(RegionType type) const;

    std::span<const u8> GetRegionData(RegionType type) const;
    std::span<u8> GetRegionDataMutable(RegionType type);
    
    template<typename T>
    T Read(u32 address);

    template<typename T>
    void Write(u32 address, T value);

    void ClearRegion(RegionType type);
    void Clear8(u32 address);
    void Clear16(u32 address);
    void Clear32(u32 address);
    void ClearAddressRange(u32 startAddress, u32 endAddress);

    void InitROMBanks();
    void LoadBIOS(const std::vector<u8>& biosData);
    
private:
    std::unique_ptr<std::vector<u8>> bios;
    std::unique_ptr<std::vector<u8>> ewram;
    std::unique_ptr<std::vector<u8>> iwram;
    std::unique_ptr<std::vector<u8>> paletteRam;
    std::unique_ptr<std::vector<u8>> vram;
    std::unique_ptr<std::vector<u8>> oam;
    std::span<const u8> rom0View;
    std::span<const u8> rom1View;
    std::span<const u8> rom2View;
    std::unique_ptr<std::vector<u8>> sram;
    
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

#include "Core/Memory/MemoryAccess.tpp"

// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE