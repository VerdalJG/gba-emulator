#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <variant>
#include <memory>

#include "Core/GBA_ROM.hpp"
#include "Core/GBA_Memory_Helpers.hpp"

#include "Utils/Logger.hpp"

class EmulatorCore;

class GBA_Memory 
{
public:
    GBA_Memory() = delete;
    explicit GBA_Memory(EmulatorCore* core);
    ~GBA_Memory() = default;
    GBA_Memory(const GBA_Memory&) = delete; // Disable copy constructor
 
    const MemoryRegion* GetRegionFromAddress(uint32_t address) const;
    const MemoryRegion* GetRegionFromType(RegionType type) const;
    
    uint8_t Read8(uint32_t address) const;
    uint16_t Read16(uint32_t address) const;
    uint32_t Read32(uint32_t address) const;

    void Write8(uint32_t address, uint8_t value);
    void Write16(uint32_t address, uint32_t value);
    void Write32(uint32_t address, uint32_t value);

    void ClearRegion(RegionType type);
    void Clear8(uint32_t address);
    void Clear16(uint32_t address);
    void Clear32(uint32_t address);
    void ClearAddressRange(uint32_t startAddress, uint32_t endAddress);

    void LoadROM(const std::vector<uint8_t>& romData);
    void LoadBIOS(const std::vector<uint8_t>& biosData);

    void ResetSIORegisters();
    void ResetSoundRegisters();
    void ResetOtherIORegisters();

    const GBA_WaitstateController& GetWaitstateController() const { return waitstateController; } 
    
private:
    // General internal memory
    MemoryRegion bios = MemoryRegion(Permissions::ReadOnly, BIOS_START, BIOS_SIZE, RegionType::BIOS);
    MemoryRegion ewram = MemoryRegion(Permissions::ReadWrite, EWRAM_START, EWRAM_SIZE, RegionType::EWRAM); // External work RAM
    MemoryRegion iwram = MemoryRegion(Permissions::ReadWrite, IWRAM_START, IWRAM_SIZE, RegionType::IWRAM); // Internal work RAM
    MemoryRegion ioRegisters = MemoryRegion(Permissions::Varies, IOREGISTERS_START, IOREGISTERS_SIZE, RegionType::IORegisters);

    // Internal display memory
    MemoryRegion paletteRam = MemoryRegion(Permissions::ReadWrite, PALETTE_RAM_START, PALETTE_RAM_SIZE, RegionType::PaletteRAM);
    MemoryRegion vram = MemoryRegion(Permissions::ReadWrite, VRAM_START, VRAM_SIZE, RegionType::VRAM); // Video RAM
    MemoryRegion oam = MemoryRegion(Permissions::ReadWrite, OAM_START, OAM_SIZE, RegionType::OAM); // Object-Attribute RAM
    
    // External memory (cartridge)

    // ROM0/ROM1/ROM2 all point to the same ROM data but differ by waitstate timing.
    // ROM1 and ROM2 are mirrors of ROM0 at different addresses (for access timing differences).

    MemoryRegion rom0 = MemoryRegion(Permissions::ReadOnly, ROM0_START, RegionType::ROM0);
    MemoryRegion rom1 = MemoryRegion(Permissions::ReadOnly, ROM1_START, RegionType::ROM1);
    MemoryRegion rom2 = MemoryRegion(Permissions::ReadOnly, ROM2_START, RegionType::ROM2);
    MemoryRegion sram = MemoryRegion(Permissions::ReadWrite, SRAM_START, SRAM_SIZE, RegionType::SRAM);

    // The GBA has unused memory area after the SRAM, which goes from 0x10000000 to 0xFFFFFFFF

    LastBusAccess lastAccess; // For open-bus emulation

    std::unique_ptr<GBA_ROM> rom;
    EmulatorCore* core; 
    GBA_WaitstateController waitstateController;

    void Log(const std::string& message, LogType logType, const char* functionName = "") const;

    template <typename T>
    T Read(uint32_t address, AccessSize size) const;

    template <typename T>
    void Write(uint32_t address, T value);

    template <typename T>
    T FillFromLastBusAccess() const;

};

#include "GBA_Memory.tpp"

// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE