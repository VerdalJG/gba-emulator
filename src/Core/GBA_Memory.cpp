#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_ROM.hpp"

#include <stdexcept>
#include <assert.h>


GBA_Memory::GBA_Memory(EmulatorCore *core) : core(core)
{
    rom = std::make_unique<GBA_ROM>(core);
}

const MemoryRegion* GBA_Memory::GetRegionFromAddress(uint32_t address) const
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

const MemoryRegion* GBA_Memory::GetRegionFromType(RegionType type) const
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
    return Read<uint8_t>(address, AccessSize::Byte);
}

uint16_t GBA_Memory::Read16(uint32_t address)
{
    return Read<uint16_t>(address, AccessSize::Halfword);
}

uint32_t GBA_Memory::Read32(uint32_t address)
{
    return Read<uint32_t>(address, AccessSize::Word);
}   

void GBA_Memory::Write8(uint32_t address, uint8_t value)
{
    Write<uint8_t>(address, value);
}

void GBA_Memory::Write16(uint32_t address, uint32_t value)
{
    Write<uint16_t>(address, value);
}

void GBA_Memory::Write32(uint32_t address, uint32_t value)
{
    Write<uint32_t>(address, value);
}

void GBA_Memory::LoadROM(const std::vector<uint8_t>& romData)
{
    assert(rom && "ROM object is null in GBA_Memory::LoadROM()");

    rom->LoadROM(romData);

    rom0.data = rom->GetROMData();
    rom1.data = rom->GetROMData();
    rom2.data = rom->GetROMData();

    rom->PrintROMInfo();
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
    const MemoryRegion* region = GetRegionFromType(type); 
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
    const MemoryRegion* region = GetRegionFromAddress(startAddress);

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

void GBA_Memory::Log(const std::string &message, LogType logType, const char* functionName) const
{
    core->Log(message, logType, functionName);
}
