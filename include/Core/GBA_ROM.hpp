#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

class EmulatorCore;

struct ROM_Header
{
    // In order of memory addresses:

    uint32_t entryPoint;
    uint8_t nintendoLogo[156];
    char gameTitle[12];
    char gameCode[4];
    char makerCode[2];
    uint8_t fixedValue;
    uint8_t mainUnitCode;
    uint8_t deviceType;
    uint8_t reserved[7];
    uint8_t softwareVersion;
    uint8_t complementCheck;
    uint16_t reserved2;
};

class GBA_ROM
{
public: 
    GBA_ROM() = default;
    explicit GBA_ROM(EmulatorCore* core);

    // Load ROM data
    void LoadROM(const std::vector<uint8_t>& romData);
    

    // Read methods
    uint8_t Read8(uint32_t address) const;
    uint16_t Read16(uint32_t address) const;
    uint32_t Read32(uint32_t address) const;

    // ROM info
    std::shared_ptr<std::vector<uint8_t>> GetROMData() const { return rom; }
    size_t GetSize() const { return rom->size(); }
    const ROM_Header& GetHeader() const { return header; }

private:
    std::shared_ptr<std::vector<uint8_t>> rom;
    ROM_Header header;
    EmulatorCore* core;

    void ParseHeader();
};