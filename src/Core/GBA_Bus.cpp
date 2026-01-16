#include "Core/GBA_Bus.hpp"
#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"

GBA_Bus::GBA_Bus(EmulatorCore* core, GBA_Memory& memory) : core(core), memory(memory)
{
    
}

GBA_MemoryRegionType GBA_Bus::GetRegionFromAddress(uint32_t address) const 
{
    switch (address >> 24)
    {
        case 0x00: return GBA_MemoryRegionType::BIOS;
        case 0x02: return GBA_MemoryRegionType::EWRAM;
        case 0x03: return GBA_MemoryRegionType::IWRAM;
        case 0x04: return GBA_MemoryRegionType::IO;
        case 0x05: return GBA_MemoryRegionType::PaletteRAM;
        case 0x06: return GBA_MemoryRegionType::VRAM;
        case 0x07: return GBA_MemoryRegionType::OAM;
        case 0x08: case 0x09: return GBA_MemoryRegionType::ROM0;
        case 0x0A: case 0x0B: return GBA_MemoryRegionType::ROM1;
        case 0x0C: case 0x0D: return GBA_MemoryRegionType::ROM2;
        case 0x0E: return GBA_MemoryRegionType::SRAM;

        default: return GBA_MemoryRegionType::Invalid;
    }

    // TODO:
    // Reading from BIOS region:

    // If reading from bios memory the GBA allows to read opcodes or data only if the program counter 
    // is located inside of the BIOS area. If the program counter is not in the BIOS area,
    // reading will return the most recent successfully fetched BIOS opcode

    // Reading from Unused memory regions

    // Accessing unused memory at 00004000h-01FFFFFFh, and 10000000h-FFFFFFFFh (and 02000000h-03FFFFFFh when RAM is 
    // disabled via Port 4000800h) returns the recently pre-fetched opcode.
}

uint8_t GBA_Bus::Read8(uint32_t address, uint32_t& cycles) 
{
    GBA_MemoryRegionType regionType = GetRegionFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (region == nullptr)
    {
        return lastValue & 0xFF; // Return the low 8 bits
    }
    
    if (!region->IsValidAccess(address, AccessType::Read, BusAccessSize::Byte))
    {
        const std::string message = "Invalid Read8 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);
        return lastValue & 0xFF; // Return the low 8 bits
    }

    uint8_t readValue = memory.Read8(address);
    //cycles = region->
    lastValue = readValue;
    lastSize = BusAccessSize::Byte;

    return readValue;
}

uint16_t GBA_Bus::Read16(uint32_t address, uint32_t& cycles) 
{ 
    GBA_MemoryRegionType regionType = GetRegionFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (region == nullptr)
    {
        return lastValue & 0xFFFF; // Return the low 16 bits
    }
    
    if (!region->IsValidAccess(address, AccessType::Read, BusAccessSize::Halfword))
    {
        const std::string message = "Invalid Read16 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);
        return lastValue & 0xFFFF; // Return the low 16 bits
    }

    uint16_t readValue = memory.Read16(address);
    //cycles = region->
    lastValue = readValue;
    lastSize = BusAccessSize::Halfword;

    return readValue;
}

uint32_t GBA_Bus::Read32(uint32_t address, uint32_t& cycles) 
{ 
    GBA_MemoryRegionType regionType = GetRegionFromAddress(address);
    const GBA_MemoryRegion* region = memory.GetRegionFromType(regionType);

    if (region == nullptr)
    {
        return lastValue; // Return the full value
    }
    
    if (!region->IsValidAccess(address, AccessType::Read, BusAccessSize::Word))
    {
        const std::string message = "Invalid Read32 at address: " + std::to_string(address);
        core->Log(message, LogType::Warning);
        return lastValue; // Return the full value
    }

    uint32_t readValue = memory.Read32(address);
    //cycles = region->
    lastValue = readValue;
    lastSize = BusAccessSize::Word;

    return readValue; 
}

void GBA_Bus::Write(uint32_t addr, uint32_t value, BusAccessSize size) 
{
    
}
