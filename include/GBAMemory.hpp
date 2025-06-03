#pragma once

#include <cstdint>
#include <vector>
#include <array>

using namespace std;

// Memory map sizes for GBA
constexpr size_t BIOS_SIZE = 0x4000; // 16KB
constexpr size_t EWRAM_SIZE = 0x40000; // External work RAM / On GBA mainboard RAM - 256KB
constexpr size_t IWRAM_SIZE = 0x8000; // Internal work RAM / On CPU chip RAM - 32KB - Fastest RAM
constexpr size_t PALETTE_RAM_SIZE = 0x400; // Palette RAM - 1KB
constexpr size_t VRAM_SIZE = 0x18000; // Video RAM - 144KB
constexpr size_t OAM_SIZE = 0x400; // Object Attribute Memory - 1KB
constexpr size_t ROM_MAX_SIZE = 0x2000000; // Maximum ROM size - 32MB, most games are less than 16MB

// TODO: vectors of ewram, iwrm, rom
array<uint8_t, BIOS_SIZE> bios; // BIOS
array<uint8_t, EWRAM_SIZE> ewram; // External work RAM
array<uint8_t, IWRAM_SIZE> iwrm; // Internal work RAM
array<uint8_t, VRAM_SIZE> vram; // Video RAM
array<uint8_t, ROM_MAX_SIZE> rom; // ROM

// TODO: Functions that interact with memory - memory init, load rom, read and write

// https://problemkaputt.de/gbatek-gba-memory-map.htm REFERENCE