#pragma once

#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Core/IO/Addresses.hpp"

#include <array>

constexpr auto BuildIOPermissionTable() 
{
    constexpr HalfwordPermissions DEFAULT = {0b00, 0b00};
    constexpr HalfwordPermissions RW  = {0b11, 0b11};
    constexpr HalfwordPermissions RO  = {0b11, 0b00};
    constexpr HalfwordPermissions WO  = {0b00, 0b11};
    constexpr HalfwordPermissions POSTFLG_HALTCNT = {0b01, 0b10};

    std::array<HalfwordPermissions, IO_TABLE_SIZE / 2> table{};
    table.fill(DEFAULT);

    auto ioIndex = [](u32 address) -> uint 
    {
        return (address - IO_START) >> 1; // Shift to get halfword index
    };
    
    ///
    // LCD Registers
    ///
    table[ioIndex(DISPCNT)]     = RW;
    table[ioIndex(GREENSWAP)]   = RW;
    table[ioIndex(DISPSTAT)]    = RW;
    table[ioIndex(VCOUNT)]      = RO;

    table[ioIndex(BG0CNT)]      = RW;
    table[ioIndex(BG1CNT)]      = RW;
    table[ioIndex(BG2CNT)]      = RW;
    table[ioIndex(BG3CNT)]      = RW;

    table[ioIndex(BG0HOFS)]     = WO;
    table[ioIndex(BG0VOFS)]     = WO;
    table[ioIndex(BG1HOFS)]     = WO;
    table[ioIndex(BG1VOFS)]     = WO;
    table[ioIndex(BG2HOFS)]     = WO;
    table[ioIndex(BG2VOFS)]     = WO;
    table[ioIndex(BG3HOFS)]     = WO;
    table[ioIndex(BG3VOFS)]     = WO;

    table[ioIndex(BG2PA)]       = WO;
    table[ioIndex(BG2PB)]       = WO;
    table[ioIndex(BG2PC)]       = WO;
    table[ioIndex(BG2PD)]       = WO;

    table[ioIndex(BG2X_L)]      = WO;
    table[ioIndex(BG2X_H)]      = WO;
    table[ioIndex(BG2X_L)]      = WO;
    table[ioIndex(BG2X_H)]      = WO;

    table[ioIndex(BG3PA)]       = WO;
    table[ioIndex(BG3PB)]       = WO;
    table[ioIndex(BG3PC)]       = WO;
    table[ioIndex(BG3PD)]       = WO;

    table[ioIndex(BG3X_L)]      = WO;
    table[ioIndex(BG3X_H)]      = WO;
    table[ioIndex(BG3X_L)]      = WO;
    table[ioIndex(BG3X_H)]      = WO;

    table[ioIndex(WIN0H)]       = WO;
    table[ioIndex(WIN1H)]       = WO;
    table[ioIndex(WIN0V)]       = WO;
    table[ioIndex(WIN1V)]       = WO;

    table[ioIndex(WININ)]       = RW;
    table[ioIndex(WINOUT)]      = RW;

    table[ioIndex(MOSAIC)]      = WO;
    table[ioIndex(BLDCNT)]      = RW;
    table[ioIndex(BLDALPHA)]    = RW;
    table[ioIndex(BLDY)]        = WO;

    ///
    // Sound Registers
    ///
    table[ioIndex(SOUND1CNT_L)] = RW;
    table[ioIndex(SOUND1CNT_H)] = RW;
    table[ioIndex(SOUND1CNT_X)] = RW;

    table[ioIndex(SOUND2CNT_L)] = RW;
    table[ioIndex(SOUND2CNT_H)] = RW;

    table[ioIndex(SOUND3CNT_L)] = RW;
    table[ioIndex(SOUND3CNT_H)] = RW;
    table[ioIndex(SOUND3CNT_X)] = RW;

    table[ioIndex(SOUND4CNT_L)] = RW;
    table[ioIndex(SOUND4CNT_H)] = RW;

    table[ioIndex(SOUNDCNT_L)]  = RW;
    table[ioIndex(SOUNDCNT_H)]  = RW;
    table[ioIndex(SOUNDCNT_X)]  = RW;

    table[ioIndex(SOUNDBIAS)]   = RW;

    table[ioIndex(WAVE_RAM0_L)] = RW;
    table[ioIndex(WAVE_RAM0_H)] = RW;

    table[ioIndex(WAVE_RAM1_L)] = RW;
    table[ioIndex(WAVE_RAM1_H)] = RW;

    table[ioIndex(WAVE_RAM2_L)] = RW;
    table[ioIndex(WAVE_RAM2_H)] = RW;

    table[ioIndex(WAVE_RAM3_L)] = RW;
    table[ioIndex(WAVE_RAM3_H)] = RW;

    table[ioIndex(FIFO_A + 2)]  = WO;
    table[ioIndex(FIFO_B + 2)]  = WO;

    ///
    // DMA Registers
    ///
    table[ioIndex(DMA0SAD)]     = WO;
    table[ioIndex(DMA0SAD + 2)] = WO;
    table[ioIndex(DMA0DAD)]     = WO;
    table[ioIndex(DMA0DAD + 2)] = WO;
    table[ioIndex(DMA0CNT_L)]   = WO;
    table[ioIndex(DMA0CNT_H)]   = RW;

    table[ioIndex(DMA1SAD)]     = WO;
    table[ioIndex(DMA1SAD + 2)] = WO;
    table[ioIndex(DMA1DAD)]     = WO;
    table[ioIndex(DMA1DAD + 2)] = WO;
    table[ioIndex(DMA1CNT_L)]   = WO;
    table[ioIndex(DMA1CNT_H)]   = RW;

    table[ioIndex(DMA2SAD)]     = WO;
    table[ioIndex(DMA2SAD + 2)] = WO;
    table[ioIndex(DMA2DAD)]     = WO;
    table[ioIndex(DMA2DAD + 2)] = WO;
    table[ioIndex(DMA2CNT_L)]   = WO;
    table[ioIndex(DMA2CNT_H)]   = RW;

    table[ioIndex(DMA3SAD)]     = WO;
    table[ioIndex(DMA3SAD + 2)] = WO;
    table[ioIndex(DMA3DAD)]     = WO;
    table[ioIndex(DMA3DAD + 2)] = WO;
    table[ioIndex(DMA3CNT_L)]   = WO;
    table[ioIndex(DMA3CNT_H)]   = RW;

    ///
    // Timer Registers
    ///
    table[ioIndex(TM0CNT_L)]    = RW;
    table[ioIndex(TM0CNT_H)]    = RW;

    table[ioIndex(TM1CNT_L)]    = RW;
    table[ioIndex(TM1CNT_H)]    = RW;

    table[ioIndex(TM2CNT_L)]    = RW;
    table[ioIndex(TM2CNT_H)]    = RW;

    table[ioIndex(TM3CNT_L)]    = RW;
    table[ioIndex(TM3CNT_H)]    = RW;

    ///
    // Serial Registers Group 1
    ///
    table[ioIndex(SIODATA32)]   = RW;
    table[ioIndex(SIOMULTI0)]   = RW;
    table[ioIndex(SIOMULTI1)]   = RW;
    table[ioIndex(SIOMULTI2)]   = RW;
    table[ioIndex(SIOCNT)]      = RW;
    table[ioIndex(SIOMLT_SEND)] = RW; // Shared with SIODATA8

    ///
    // Keypad Registers
    ///
    table[ioIndex(KEYINPUT)]    = RO;
    table[ioIndex(KEYCNT)]      = RW;

    ///
    // Serial Registers Group 2
    ///
    table[ioIndex(RCNT)]            = RW;
    table[ioIndex(JOYCNT)]          = RW;
    table[ioIndex(JOY_RECV)]        = RW;
    table[ioIndex(JOY_RECV + 2)]    = RW;
    table[ioIndex(JOY_TRANS)]       = RW;
    table[ioIndex(JOY_TRANS + 2)]   = RW;
    table[ioIndex(JOYSTAT)]         = RO;

    ///
    // Interrupt, Waitstates, Power Down Control Registers
    ///
    table[ioIndex(IE)] = RW;
    table[ioIndex(IF)] = RW;
    table[ioIndex(WAITCNT)] = RW;
    table[ioIndex(IME)] = RW;
    table[ioIndex(POSTFLG)] = POSTFLG_HALTCNT;
    table[ioIndex(UNKNOWN)] = {0b01, 0b01};
    table[ioIndex(IMC)]     = RW;
    table[ioIndex(IMC + 2)] = RW;

    return table;
}