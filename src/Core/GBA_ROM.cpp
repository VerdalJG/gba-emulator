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

uint8_t GBA_ROM::Read8(uint32_t address) const
{
    uint8_t value = (*rom)[address];
    return value;
}

uint16_t GBA_ROM::Read16(uint32_t address) const
{
    const auto& romData = *rom;
    return romData[address] | (romData[address + 1] << 8);
}

uint32_t GBA_ROM::Read32(uint32_t address) const
{
    const auto& romData = *rom;
    return romData[address]
    | (romData[address + 1] << 8)
    | (romData[address + 2] << 16)
    | (romData[address + 3] << 24);
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
}
