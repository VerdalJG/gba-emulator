#include "Core/GBA_ROM.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_NintendoLogo.hpp"
#include "Core/GBA_Memory.hpp"

GBA_ROM::GBA_ROM(EmulatorCore *core) : core(core)
{
}

void GBA_ROM::LoadROM(const std::vector<uint8_t>& romData)
{
    rom = std::make_shared<std::vector<uint8_t>>(romData);

    // Ensure rom data is at least 32 MB - 
    // NOT CORRECT MIRRORING BEHAVIOR BUT should be simple and stable
    if (rom->size() < ROM_BANK_SIZE)
    {
        rom->resize(ROM_BANK_SIZE, 0xFF);
    }
}

void GBA_ROM::PrintROMInfo()
{
    printf("Game Title: %.12s\n", header.gameTitle);
    printf("Game Code: %.4s\n", header.gameCode);
    printf("Maker: %.2s\n", header.makerCode);
    printf("Version: %u\n", header.softwareVersion);
}

void GBA_ROM::ParseHeader()
{
    if (rom->size() < sizeof(ROM_Header))
    {
        valid = false;
        return;
    }

    std::memcpy(&header, rom->data(), sizeof(ROM_Header));

    if (usingOnlyOfficialSoftware)
    {
        valid = std::memcmp(NintendoLogo, header.nintendoLogo, 156) == 0;
    }
    
    valid = true;
}
