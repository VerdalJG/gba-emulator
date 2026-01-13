#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <variant>
#include <memory>

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
 
    const MemoryRegion* GetRegionFromAddress(uint32_t address) const;
    const MemoryRegion* GetRegionFromType(RegionType type) const;
    
    uint8_t Read8(uint32_t address);
    uint16_t Read16(uint32_t address);
    uint32_t Read32(uint32_t address);

    void Write8(uint32_t address, uint8_t value);
    void Write16(uint32_t address, uint16_t value);
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

    template <typename T>
    T GetLastBusValue() const
    {
        return static_cast<T>(lastBusAccess.value)
    }
    
    const GBA_WaitstateController& GetWaitstateController() const { return waitstateController; } 
    
private:
    // General internal memory
    MemoryRegion bios = MemoryRegion::BIOS();
    MemoryRegion ewram = MemoryRegion::EWRAM(); // External work RAM
    MemoryRegion iwram = MemoryRegion::IWRAM(); // Internal work RAM
    MemoryRegion ioRegisters = MemoryRegion::IORegisters();

    // Internal display memory
    MemoryRegion paletteRam = MemoryRegion::Palette_RAM();
    MemoryRegion vram = MemoryRegion::VRAM(); // Video RAM
    MemoryRegion oam = MemoryRegion::OAM(); // Object-Attribute RAM
    
    // External memory (cartridge)

    // ROM0/ROM1/ROM2 all point to the same ROM data but differ by waitstate timing.
    // ROM1 and ROM2 are mirrors of ROM0 at different addresses (for access timing differences).

    MemoryRegion rom0 = MemoryRegion::ROM0();
    MemoryRegion rom1 = MemoryRegion::ROM1();
    MemoryRegion rom2 = MemoryRegion::ROM2();
    MemoryRegion sram = MemoryRegion::SRAM();

    // The GBA has unused memory area after the SRAM, which goes from 0x10000000 to 0xFFFFFFFF
    LastBusAccess lastBusAccess; // For open-bus emulation

    EmulatorCore* core;
    GBA_ROM& rom;
    GBA_IO& io;
    GBA_WaitstateController waitstateController;

    void Log(const std::string& message, LogType logType, const char* functionName = "") const;

    template <typename T>
    T Read(uint32_t address, AccessSize size);

    template <typename T>
    void Write(uint32_t address, T value);

};

#include "GBA_Memory.tpp"

// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE