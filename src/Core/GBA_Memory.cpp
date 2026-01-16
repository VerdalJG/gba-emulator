#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_ROM.hpp"
#include "Core/GBA_IO.hpp"

#include <stdexcept>
#include <assert.h>


GBA_Memory::GBA_Memory(EmulatorCore *core, GBA_ROM& rom, GBA_IO& io)
    : core(core), rom(rom), io(io), 
    bios(std::make_unique<std::vector<uint8_t>>(BIOS_SIZE)), 
    ewram(std::make_unique<std::vector<uint8_t>>(EWRAM_SIZE)), 
    iwram(std::make_unique<std::vector<uint8_t>>(IWRAM_SIZE)), 
    paletteRam(std::make_unique<std::vector<uint8_t>>(PALETTE_RAM_SIZE)), 
    vram(std::make_unique<std::vector<uint8_t>>(VRAM_SIZE)), 
    oam(std::make_unique<std::vector<uint8_t>>(OAM_SIZE)), 
    rom0(std::make_unique<std::vector<uint8_t>>(ROM_BANK_SIZE)),
    rom1(std::make_unique<std::vector<uint8_t>>(ROM_BANK_SIZE)),
    rom2(std::make_unique<std::vector<uint8_t>>(ROM_BANK_SIZE)),
    sram(std::make_unique<std::vector<uint8_t>>(SRAM_SIZE))
{}

const GBA_MemoryRegion* GBA_Memory::GetRegionFromAddress(uint32_t address) const
{
    switch (address >> 24)
    {
        case 0x00: return &biosRegion;
        case 0x02: return &ewramRegion;
        case 0x03: return &iwramRegion;
        case 0x04: return &ioRegion;
        case 0x05: return &paletteRamRegion;
        case 0x06: return &vramRegion;
        case 0x07: return &oamRegion;
        case 0x08: case 0x09: return &rom0Region;
        case 0x0A: case 0x0B: return &rom1Region;
        case 0x0C: case 0x0D: return &rom2Region;
        case 0x0E: return &sramRegion;

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

const GBA_MemoryRegion* GBA_Memory::GetRegionFromType(GBA_MemoryRegionType type) const
{
    switch (type)
    {
        case GBA_MemoryRegionType::BIOS: return &biosRegion;
        case GBA_MemoryRegionType::EWRAM: return &ewramRegion;
        case GBA_MemoryRegionType::IWRAM: return &iwramRegion;
        case GBA_MemoryRegionType::IO: return &ioRegion;
        case GBA_MemoryRegionType::PaletteRAM: return &paletteRamRegion;
        case GBA_MemoryRegionType::VRAM: return &vramRegion;
        case GBA_MemoryRegionType::OAM: return &oamRegion;
        case GBA_MemoryRegionType::ROM0: return &rom0Region;
        case GBA_MemoryRegionType::ROM1: return &rom1Region;
        case GBA_MemoryRegionType::ROM2: return &rom2Region;
        case GBA_MemoryRegionType::SRAM: return &sramRegion;
        case GBA_MemoryRegionType::Invalid: return nullptr;

        default: return nullptr;
    }
}

uint8_t GBA_Memory::Read8(uint32_t address)
{
    

    return Read<uint8_t>(address, BusAccessSize::Byte);
}

uint16_t GBA_Memory::Read16(uint32_t address)
{
    return Read<uint16_t>(address, BusAccessSize::Halfword);
}

uint32_t GBA_Memory::Read32(uint32_t address)
{
    return Read<uint32_t>(address, BusAccessSize::Word);
}   

void GBA_Memory::Write8(uint32_t address, uint8_t value)
{
    Write<uint8_t>(address, value);
}

void GBA_Memory::Write16(uint32_t address, uint16_t value)
{
    Write<uint16_t>(address, value);
}

void GBA_Memory::Write32(uint32_t address, uint32_t value)
{
    Write<uint32_t>(address, value);
}

void GBA_Memory::LoadROM(const std::vector<uint8_t>& romData)
{
    std::copy(romData.begin(), romData.end(), rom0->begin());
    std::copy(romData.begin(), romData.end(), rom1->begin());
    std::copy(romData.begin(), romData.end(), rom2->begin());

    rom.PrintROMInfo();
}

void GBA_Memory::LoadBIOS(const std::vector<uint8_t>& biosData)
{
    if (biosData.size() != BIOS_SIZE)
    {
        // Error: Invalid bios data size
        throw std::runtime_error("Invalid BIOS size: expected 16 KB");
    }

    std::copy(biosData.begin(), biosData.end(), bios->begin());
}

void GBA_Memory::ClearRegion(GBA_MemoryRegionType type)
{
    std::span<uint8_t> regionData = GetRegionData(type); 
    std::fill(regionData.begin(), regionData.end(), 0);
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
    assert(startAddress <= endAddress && "Start address must be lower than the end address");

    const GBA_MemoryRegion* region = GetRegionFromAddress(startAddress);
    assert(region && "Invalid start address");
    assert(region == GetRegionFromAddress(endAddress) && "Start address and end address must pertain to the same region");

    // Ensure region is writeable (cannot clear BIOS nor ROM)
    if (region->writeMask == RNONE) return;
    if (region->type == GBA_MemoryRegionType::IO) return;

    std::span<uint8_t> regionData = GetRegionData(region->type); 

    uint32_t startOffset = startAddress - region->start;
    uint32_t endOffset = endAddress - region->start;

    assert(endOffset < regionData.size() && startOffset <= endOffset && "Attempting to clear more than one region");

    std::fill(regionData.begin() + startOffset, regionData.begin() + endOffset + 1, 0); 
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
