#include "GBAMemory.hpp"

MemoryRegion* GBAMemory::GetRegionFromAddress(uint32_t address) 
{
    switch (address >> 24)
    {
        case 0x00: return &bios;
        case 0x02: return &ewram;
        case 0x03: return &iwram;
        case 0x04: return &ioRegisters;
        case 0x05: return &paletteRam;
        case 0x06: return &vram;
        case 0x07: return &oam;
        case 0x08: case 0x09: return &rom0;
        case 0x0A: case 0x0B: return &rom1;
        case 0x0C: case 0x0D: return &rom2;
        case 0x0E: return &sram;

        default: return nullptr; // Unused or invalid address, handle accordingly
    }

    // TODO:
    // Reading from BIOS region:

    // If reading from bios memory the GBA allows to read opcodes or data only if the program counter is located inside
    // of the BIOS area. If the program counter is not in the BIOS area, reading will return the most recent successfully 
    // fetched BIOS opcode

    // Reading from Unused memory regions

    // Accessing unused memory at 00004000h-01FFFFFFh, and 10000000h-FFFFFFFFh (and 02000000h-03FFFFFFh when RAM is 
    // disabled via Port 4000800h) returns the recently pre-fetched opcode.
}

// void GBAMemory::WriteToRegion(std::vector<uint8_t> data, MemoryRegion region)
// {
    
// }

uint8_t GBAMemory::Read8(uint32_t address)
{
    MemoryRegion* region = GetRegionFromAddress(address);

    uint8_t value = 0xFF; // Default for open-bus emulation

    if (!region || !region->data)
    {
        return lastBusValue;
    }

    // TODO: Emulate waitstates

    if (region == &rom0)
    {
        // Apply waitstate 0
    }
    else if (region == &rom1)
    {
        // Apply waitstate 1
    }
    else if (region == &rom2)
    {
        // Apply waitstate 2
    }

    size_t offset = address - region->startAddress;
    if (offset >= region->data->size())
    {
        // Error: out of region bounds
        return lastBusValue;
    }
    else
    {
        value = (*region->data)[offset];
    }

    lastBusValue = value;

    return value;
}

uint16_t GBAMemory::Read16(uint32_t address)
{
    address &= ~1; // Align to nearest even byte address

    uint8_t lowerByte = Read8(address);
    uint8_t upperByte = Read8(address + 1);

    uint16_t combined = (static_cast<uint16_t>(upperByte) << 8) | lowerByte;
    return combined;
}

uint32_t GBAMemory::Read32(uint32_t address)
{
    address &= ~3; // Align to nearest address that is a multiple of 4

    uint16_t lowerBytes = Read16(address);
    uint16_t upperBytes = Read16(address + 2);

    uint32_t combined = (static_cast<uint32_t>(upperBytes) << 16) | lowerBytes;
    return combined;
}

void GBAMemory::Write8(uint32_t address, uint8_t value)
{
    MemoryRegion* region = GetRegionFromAddress(address);

    if (region == nullptr)
    {
        // Error: Attempt to write to an invalid region
        return;
    }

    if (region->permissions == Permissions::ReadOnly)
    {
        // Error: Attempt to write to a read-only region
        return;
    }
    else
    {
        size_t offset = address - region->startAddress;
        if (offset >= region->data->size())
        {
            // Error: out of region bounds
            return;
        }

        (*region->data)[offset] = value;
    }
}

void GBAMemory::Write16(uint32_t address, uint16_t value)
{
    address &= ~1; // Alignment
    Write8(address, static_cast<uint8_t>(value & 0xFF)); // Write lower byte
    Write8(address + 1, static_cast<uint8_t>((value >> 8) & 0xFF)); // Write upper byte
}

void GBAMemory::Write32(uint32_t address, uint32_t value)
{
    address &= ~3; // Alignment
    Write16(address, static_cast<uint16_t>(value & 0xFFFF)); // Write lower 16 bits
    Write16(address + 2, static_cast<uint16_t>((value >> 16) & 0xFFFF)); // Write upper 16 bits
}

void GBAMemory::LoadROM(const std::vector<uint8_t>& romData)
{
    if (romData.size() < ROM_BANK_SIZE)
    {
        // Error: Invalid rom size - Must be 32 MB
    }

    rom = romData;

    rom0.data = &rom;
    rom1.data = &rom;
    rom2.data = &rom;
}

void GBAMemory::LoadBIOS(const std::vector<uint8_t>& biosData)
{
    if (biosData.size() != 0x4000)
    {
        // Error: Invalid bios data size
    }

    std::copy(biosData.begin(), biosData.end(), (*bios.data).begin());
}

