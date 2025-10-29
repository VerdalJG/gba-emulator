#include "Core/GBA_Memory.hpp"
#include <stdexcept>
#include <assert.h>

MemoryRegion* GBA_Memory::GetRegionFromAddress(uint32_t address) 
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

    // If reading from bios memory the GBA allows to read opcodes or data only if the program counter 
    // is located inside of the BIOS area. If the program counter is not in the BIOS area,
    // reading will return the most recent successfully fetched BIOS opcode

    // Reading from Unused memory regions

    // Accessing unused memory at 00004000h-01FFFFFFh, and 10000000h-FFFFFFFFh (and 02000000h-03FFFFFFh when RAM is 
    // disabled via Port 4000800h) returns the recently pre-fetched opcode.
}

MemoryRegion* GBA_Memory::GetRegionFromType(RegionType type)
{
    switch (type)
    {
        case RegionType::BIOS: return &bios;
        case RegionType::EWRAM: return &ewram;
        case RegionType::IWRAM: return &iwram;
        case RegionType::IORegisters: return &ioRegisters;
        case RegionType::PaletteRAM: return &paletteRam;
        case RegionType::VRAM: return &vram;
        case RegionType::OAM: return &oam;
        case RegionType::ROM0: return &rom0;
        case RegionType::ROM1: return &rom1;
        case RegionType::ROM2: return &rom2;
        case RegionType::SRAM: return &sram;

        default: return nullptr; // Unused or invalid address, handle accordingly
    }
}

uint8_t GBA_Memory::Read8(uint32_t address)
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

uint32_t GBA_Memory::Read16(uint32_t address)
{
    address &= ~1; // Align to nearest even byte address

    uint8_t lowerByte = Read8(address);
    uint8_t upperByte = Read8(address + 1);

    uint32_t combined = (static_cast<uint32_t>(upperByte) << 8) | lowerByte;
    return combined;
}

uint32_t GBA_Memory::Read32(uint32_t address)
{
    address &= ~3; // Align to nearest address that is a multiple of 4

    uint32_t lowerBytes = Read16(address);
    uint32_t upperBytes = Read16(address + 2);

    uint32_t combined = (static_cast<uint32_t>(upperBytes) << 16) | lowerBytes;
    return combined;
}

void GBA_Memory::Write8(uint32_t address, uint8_t value)
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

void GBA_Memory::Write16(uint32_t address, uint32_t value)
{
    address &= ~1; // Alignment
    Write8(address, static_cast<uint8_t>(value & 0xFF)); // Write lower byte
    Write8(address + 1, static_cast<uint8_t>((value >> 8) & 0xFF)); // Write upper byte
}

void GBA_Memory::Write32(uint32_t address, uint32_t value)
{
    address &= ~3; // Alignment
    Write16(address, static_cast<uint32_t>(value & 0xFFFF)); // Write lower 16 bits
    Write16(address + 2, static_cast<uint32_t>((value >> 16) & 0xFFFF)); // Write upper 16 bits
}

void GBA_Memory::LoadROM(const std::vector<uint8_t>& romData)
{
    rom = std::make_shared<std::vector<uint8_t>>(romData);

    // Ensure rom data is at least 32 MB
    if (rom.get()->size() < ROM_BANK_SIZE)
    {
        rom.get()->resize(ROM_BANK_SIZE, 0xFF);
    }

    rom0.data = rom;
    rom1.data = rom;
    rom2.data = rom;
}

void GBA_Memory::LoadBIOS(const std::vector<uint8_t>& biosData)
{
    if (biosData.size() != BIOS_SIZE)
    {
        // Error: Invalid bios data size
        throw std::runtime_error("Invalid BIOS size: expected 16 KB");
    }

    std::copy(biosData.begin(), biosData.end(), bios.data->begin());
}

void GBA_Memory::ClearRegion(RegionType type)
{
    MemoryRegion* region = GetRegionFromType(type); 
    std::fill(region->data->begin(), region->data->end(), 0);
}

void GBA_Memory::Clear8(uint32_t address)
{
    Write8(address, 0);
}

void GBA_Memory::Clear16(uint32_t address)
{
    Write16(address, 0);
}

void GBA_Memory::Clear32(uint32_t address)
{
    Write32(address, 0);
}

void GBA_Memory::ClearAddressRange(uint32_t startAddress, uint32_t endAddress)
{
    MemoryRegion* region = GetRegionFromAddress(startAddress);

    assert(region == GetRegionFromAddress(endAddress) && "Start address and end address must pertain to the same region");

    uint32_t startOffset = startAddress - region->startAddress;
    uint32_t endOffset = endAddress - region->startAddress;

    assert(endOffset < region->data->size() && startOffset <= endOffset && "Attempting to clear more than one region");

    std::fill(region->data->begin() + startOffset, region->data->begin() + endOffset + 1, 0); 
}

void GBA_Memory::ResetSIORegisters()
{
    Write16(0x04000128, 0x8000); // RCNT, uses 0x8000 as reset value
    Clear16(0x0400012A); // JOYCNT
    Clear32(0x04000130); // JOY_RECV
    Clear32(0x04000134); // JOY_TRANS
    Clear32(0x04000138); // JOYSTAT
}

void GBA_Memory::ResetSoundRegisters()
{
    Clear16(0x04000060); // SOUND1CNT_L
    Clear16(0x04000062); // SOUND1CNT_H
    Clear32(0x04000064); // SOUND1CNT_X

    Clear32(0x04000068); // SOUND2CNT_L
    Clear32(0x0400006C); // SOUND2CNT_H

    Clear16(0x04000070); // SOUND3CNT_L
    Clear16(0x04000072); // SOUND3CNT_H
    Clear32(0x04000074); // SOUND3CNT_X

    Clear32(0x04000078); // SOUND4CNT_L
    Clear32(0x0400007C); // SOUND4CNT_H

    Clear16(0x04000080); // SOUNDCNT_L
    Clear16(0x04000082); // SOUNDCNT_H
    Clear32(0x04000084); // SOUNDCNT_X

    Write16(0x04000088, 0x0200); // SOUNDBIAS

    Clear32(0x040000A0); // FIFO_A_L
    Clear32(0x040000A4); // FIFO_B_L
}

void GBA_Memory::ResetOtherIORegisters()
{
    // --- Display ---
    Write16(0x04000000, 0x0080);                // DISPCNT: forced blank enabled
    Clear16(0x04000004);                        // DISPSTAT
    
    // skip VCOUNT (0x04000006) – read-only
    ClearAddressRange(0x04000008, 0x04000034);  // BGxCNT/HOFS/VOFS, WIN, MOSAIC, BLDCNT, BLDALPHA, BLDY

    // --- DMA 0–3 ---
    ClearAddressRange(0x040000B0, 0x040000DE);

    // --- Timers 0–3 ---
    ClearAddressRange(0x04000100, 0x0400010E);

    // --- Keypad ---
    Clear16(0x04000130);                        // KEYCNT

    // --- Interrupts & system control ---
    ClearAddressRange(0x04000200, 0x04000208);  // IE, IF, WAITCNT, IME
}
