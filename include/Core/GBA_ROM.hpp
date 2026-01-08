#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

class EmulatorCore;

// We pack the ROM header so the compiler does not
// any padding to ROM_Header's data and make sure the memory
// is continuous

#pragma pack(push, 1)
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
#pragma pack(pop)

class GBA_ROM
{
public: 
    GBA_ROM() = delete;
    explicit GBA_ROM(EmulatorCore* core);
    ~GBA_ROM() = default;

    // Load ROM data
    void LoadROM(const std::vector<uint8_t>& romData);

    // ROM info
    std::shared_ptr<std::vector<uint8_t>> GetROMData() const { return romData; }
    size_t GetSize() const { return romData->size(); }
    const ROM_Header& GetHeader() const { return header; }
    void PrintROMInfo();

private:
    std::shared_ptr<std::vector<uint8_t>> romData;
    ROM_Header header;
    EmulatorCore* core;
    bool valid;
    bool usingOnlyOfficialSoftware = false;

    void ParseHeader();
};