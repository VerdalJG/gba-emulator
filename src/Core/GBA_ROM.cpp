#include "Core/GBA_ROM.hpp"
#include "Core/EmulatorCore.hpp"

GBA_ROM::GBA_ROM(EmulatorCore *core) : core(core)
{
}

void GBA_ROM::LoadROM(const std::vector<uint8_t>& romData)
{
    rom = std::make_shared<std::vector<uint8_t>>(romData);
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
    | (romData[address + 3] << 32);
}
