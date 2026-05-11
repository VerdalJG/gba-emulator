#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <memory>

#include "Utils/Integers.hpp"

class EmulatorCore;

// We pack the ROM header so the compiler does not
// any padding to ROM_Header's data and make sure the memory
// is continuous

#pragma pack(push, 1)
struct ROM_Header
{
    // In order of memory addresses:

    u32 entryPoint;
    u8 nintendoLogo[156];
    char gameTitle[12];
    char gameCode[4];
    char makerCode[2];
    u8 fixedValue;
    u8 mainUnitCode;
    u8 deviceType;
    u8 reserved[7];
    u8 softwareVersion;
    u8 complementCheck;
    u16 reserved2;
};
#pragma pack(pop)

class GBA_ROM
{
public: 
    GBA_ROM() = delete;
    explicit GBA_ROM(EmulatorCore* core);
    ~GBA_ROM() = default;

    // Load ROM data
    void LoadROM(const std::vector<u8>& romData);

    // ROM info
    //std::unique_ptr<const std::vector<u8>> GetROMData() const { return romData; }
    size_t GetSize() const { return romData->size(); }
    const ROM_Header& GetHeader() const { return header; }
    void PrintROMInfo();
    u16 Read16(u32 address);
    u32 Read32(u32 address);

private:
    std::unique_ptr<const std::vector<u8>> romData;
    ROM_Header header;
    EmulatorCore* core;
    bool valid;
    bool usingOnlyOfficialSoftware = false;

    void ParseHeader();
};