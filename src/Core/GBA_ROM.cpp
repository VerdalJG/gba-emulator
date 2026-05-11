#include "Core/GBA_ROM.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_NintendoLogo.hpp"
#include "Core/Memory/GBA_Memory.hpp"

#include <assert.h>

GBA_ROM::GBA_ROM(EmulatorCore *core) : core(core)
{
}

void GBA_ROM::LoadROM(const std::vector<uint8_t>& romData)
{
    this->romData = std::make_unique<std::vector<uint8_t>>(romData);
    ParseHeader();
    PrintROMInfo();
}

void GBA_ROM::PrintROMInfo()
{
    ParseHeader();
    printf("Game Title: %.12s\n", header.gameTitle);
    printf("Game Code: %.4s\n", header.gameCode);
    printf("Maker: %.2s\n", header.makerCode);
    printf("Version: %u\n", header.softwareVersion);
}

u16 GBA_ROM::Read16(u32 address) 
{ 
    u16 readValue = 0;

    readValue |= static_cast<u16>((*romData)[address]);
    readValue |= static_cast<u16>((*romData)[address + 1]) << 8;

    return readValue;
}

u32 GBA_ROM::Read32(u32 address) 
{ 
    u32 readValue = 0;

    readValue |= static_cast<u32>((*romData)[address]);
    readValue |= static_cast<u32>((*romData)[address + 1]) << 8;
    readValue |= static_cast<u32>((*romData)[address + 2]) << 16;
    readValue |= static_cast<u32>((*romData)[address + 3]) << 24;

    return readValue; 
}


void GBA_ROM::ParseHeader()
{
    if (romData->size() < sizeof(ROM_Header))
    {
        valid = false;
        return;
    }

    std::memcpy(&header, romData->data(), sizeof(ROM_Header));

    if (usingOnlyOfficialSoftware)
    {
        valid = std::memcmp(NintendoLogo, header.nintendoLogo, 156) == 0;
        assert(valid && "Not original Nintendo ROM");
    }
    else
    {
        valid = true;
    }
}
