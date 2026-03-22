#include "Core/GBA_IO.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_IO_Helpers.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/Memory/GBA_WaitstateController.hpp"
#include "Core/Memory/GBA_Memory_Helpers.hpp"

GBA_IO::GBA_IO(EmulatorCore* core) : core(core)
{
    std::fill(std::begin(ioRegisters), std::end(ioRegisters), nullptr);
    PopulateIORegistersMap();
}

void GBA_IO::AttachSubsystems(GBA_CPU* cpu, GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma, 
    GBA_TimerController* timers, GBA_InterruptController* interrupts, GBA_Keypad* keypad,
    GBA_WaitstateController* waitstates) 
{
    this->ppu = ppu;
    this->apu = apu;
    this->dma = dma;
    this->timers = timers;
    this->interrupts = interrupts;
    this->keypad = keypad;
    this->waitstates = waitstates;
}

u8 GBA_IO::Read8(u32 address) 
{
    IORegister* reg = ioRegisters[address];
    if (!reg || !reg->readable) return OPEN_BUS;

    // Accounting for little-endianess
    uint byteOffset = (address - reg->address);
    uint bitOffset = byteOffset * 8; 
    u8 read = static_cast<u8>((reg->value >> bitOffset) & 0xFF);

    if (reg->onRead) reg->onRead(address);
    return { read, true };
}

u16 GBA_IO::Read16(u32 address)
{
    IORegister* reg = ioRegisters[address];
    if (!reg || !reg->readable) return OPEN_BUS;

    uint byteOffset = (address - reg->address);
    uint bitOffset = byteOffset * 8; 
    u16 read = static_cast<u16>((reg->value >> bitOffset) & 0xFFFF);

    if (reg->onRead) reg->onRead(address);
    return { read, true };
}

u32 GBA_IO::Read32(u32 address) 
{
    auto lo = Read16(address);
    auto hi = Read16(address + 2);

    u32 value = (hi.value << 16) | lo.value;
    bool valid = lo.valid && hi.valid;

    return { value, valid };
}

void GBA_IO::Write8(u32 address, u8 value) 
{
    IORegister* reg = ioRegisters[address];
    if (!reg || !reg->writeable) return;

    u32 shift = (address - reg->address) * 8;
    reg->value &= ~(0xFFu << shift); // clear the target byte
    reg->value |= (value << shift); // set the new byte

    if (reg->onWrite) reg->onWrite(address, reg->value);
}

void GBA_IO::Write16(u32 address, u16 value) 
{
    IORegister* reg = ioRegisters[address];
    if (!reg || !reg->writeable) return;

    u32 shift = (address - reg->address) * 8;
    reg->value &= ~(0xFFFFu << shift); // clear the target halfword
    reg->value |= (value << shift); // set the new halfword

    if (reg->onWrite) reg->onWrite(address, reg->value);
}

void GBA_IO::Write32(u32 address, u32 value) 
{
    Write16(address, value & 0xFFFF);
    Write16(address + 2, value >> 16);
}

bool GBA_IO::IsValidIORegister(u32 address) 
{ 
    constexpr u32 boundary = IO_START + IO_SIZE;
    if (address <= boundary)
    {
        return ioRegisters[address - IO_START] != nullptr; // Found a register
    }

    if ((address & 0xFFFF) == 0x0800)
    {
        return true; // This maps to Internal memory control, it is an undocumented register but it is a valid register
    }

    return false;
}

void GBA_IO::PopulateIORegistersMap()
{
    auto addRegister = [this](IORegister& reg) -> void 
    {
        for (int i = 0; i < static_cast<size_t>(reg.width); i++)
        {
            ioRegisters[reg.address + i] = &reg;
        }
    };

    

    // ===============================
    // LCD Registers
    // ===============================
    addRegister(lcdRegisters.DISPCNT);
    addRegister(lcdRegisters.GREENSWAP);
    addRegister(lcdRegisters.DISPSTAT);
    addRegister(lcdRegisters.VCOUNT);

    addRegister(lcdRegisters.BG0CNT);
    addRegister(lcdRegisters.BG1CNT);
    addRegister(lcdRegisters.BG2CNT);
    addRegister(lcdRegisters.BG3CNT);

    addRegister(lcdRegisters.BG0HOFS);
    addRegister(lcdRegisters.BG0VOFS);
    addRegister(lcdRegisters.BG1HOFS);
    addRegister(lcdRegisters.BG1VOFS);
    addRegister(lcdRegisters.BG2HOFS);
    addRegister(lcdRegisters.BG2VOFS);
    addRegister(lcdRegisters.BG3HOFS);
    addRegister(lcdRegisters.BG3VOFS);

    addRegister(lcdRegisters.BG2PA);
    addRegister(lcdRegisters.BG2PB);
    addRegister(lcdRegisters.BG2PC);
    addRegister(lcdRegisters.BG2PD);

    addRegister(lcdRegisters.BG2X);
    addRegister(lcdRegisters.BG2Y);

    addRegister(lcdRegisters.BG3PA);
    addRegister(lcdRegisters.BG3PB);
    addRegister(lcdRegisters.BG3PC);
    addRegister(lcdRegisters.BG3PD);

    addRegister(lcdRegisters.BG3X);
    addRegister(lcdRegisters.BG3Y);

    addRegister(lcdRegisters.WIN0H);
    addRegister(lcdRegisters.WIN1H);
    addRegister(lcdRegisters.WIN0V);
    addRegister(lcdRegisters.WIN1V);

    addRegister(lcdRegisters.WININ);
    addRegister(lcdRegisters.WINOUT);

    addRegister(lcdRegisters.MOSAIC);

    addRegister(lcdRegisters.BLDCNT);
    addRegister(lcdRegisters.BLDALPHA);
    addRegister(lcdRegisters.BLDY);

    // ===============================
    // Sound Registers
    // ===============================
    addRegister(soundRegisters.SOUND1CNT_L);
    addRegister(soundRegisters.SOUND1CNT_H);
    addRegister(soundRegisters.SOUND1CNT_X);

    addRegister(soundRegisters.SOUND2CNT_L);
    addRegister(soundRegisters.SOUND2CNT_H);

    addRegister(soundRegisters.SOUND3CNT_L);
    addRegister(soundRegisters.SOUND3CNT_H);
    addRegister(soundRegisters.SOUND3CNT_X);

    addRegister(soundRegisters.SOUND4CNT_L);
    addRegister(soundRegisters.SOUND4CNT_H);

    addRegister(soundRegisters.SOUNDCNT_L);
    addRegister(soundRegisters.SOUNDCNT_H);
    addRegister(soundRegisters.SOUNDCNT_X);

    addRegister(soundRegisters.SOUNDBIAS);
    addRegister(soundRegisters.WAVE_RAM);

    addRegister(soundRegisters.FIFO_A);
    addRegister(soundRegisters.FIFO_B);

    // ===============================
    // DMA Registers
    // ===============================
    addRegister(dmaRegisters.DMA0SAD);
    addRegister(dmaRegisters.DMA0DAD);
    addRegister(dmaRegisters.DMA0CNT_L);
    addRegister(dmaRegisters.DMA0CNT_H);

    addRegister(dmaRegisters.DMA1SAD);
    addRegister(dmaRegisters.DMA1DAD);
    addRegister(dmaRegisters.DMA1CNT_L);
    addRegister(dmaRegisters.DMA1CNT_H);

    addRegister(dmaRegisters.DMA2SAD);
    addRegister(dmaRegisters.DMA2DAD);
    addRegister(dmaRegisters.DMA2CNT_L);
    addRegister(dmaRegisters.DMA2CNT_H);

    addRegister(dmaRegisters.DMA3SAD);
    addRegister(dmaRegisters.DMA3DAD);
    addRegister(dmaRegisters.DMA3CNT_L);
    addRegister(dmaRegisters.DMA3CNT_H);

    // ===============================
    // Timer Registers
    // ===============================
    addRegister(timerRegisters.TM0CNT_L);
    addRegister(timerRegisters.TM0CNT_H);
    addRegister(timerRegisters.TM1CNT_L);
    addRegister(timerRegisters.TM1CNT_H);
    addRegister(timerRegisters.TM2CNT_L);
    addRegister(timerRegisters.TM2CNT_H);
    addRegister(timerRegisters.TM3CNT_L);
    addRegister(timerRegisters.TM3CNT_H);

    // ===============================
    // Serial Registers
    // ===============================
    addRegister(serialRegisters.SIODATA32);
    addRegister(serialRegisters.SIOMULTI0);
    addRegister(serialRegisters.SIOMULTI1);
    addRegister(serialRegisters.SIOMULTI2);
    addRegister(serialRegisters.SIOMULTI3);
    addRegister(serialRegisters.SIOCNT);
    addRegister(serialRegisters.SIOMLT_SEND);
    addRegister(serialRegisters.SIODATA8);
    addRegister(serialRegisters.RCNT);
    addRegister(serialRegisters.IR);
    addRegister(serialRegisters.JOYCNT);
    addRegister(serialRegisters.JOY_RECV);
    addRegister(serialRegisters.JOY_TRANS);
    addRegister(serialRegisters.JOYSTAT);

    // ===============================
    // Keypad Registers
    // ===============================
    addRegister(keypadRegisters.KEYINPUT);
    addRegister(keypadRegisters.KEYCNT);

    // ===============================
    // Interrupt Registers
    // ===============================
    addRegister(interruptRegisters.IE);
    addRegister(interruptRegisters.IF);
    addRegister(interruptRegisters.IME);

    // ===============================
    // Misc Registers
    // ===============================
    addRegister(miscRegisters.WAITCNT);
    addRegister(miscRegisters.POSTFLG);
    addRegister(miscRegisters.HALTCNT);
}

// void GBA_IO::SetupCallbacks() 
// {
//     SetupLCDReadCallbacks();
//     SetupLCDWriteCallbacks();

//     // SetupTimerReadCallbacks();
//     // SetupTimerWriteCallbacks();

//     // SetupDMAReadCallbacks();
//     // SetupDMAWriteCallbacks();

//     // SetupSoundReadCallbacks();
//     // SetupSoundWriteCallbacks();

//     // SetupInterruptCallbacks();
// }

// void GBA_IO::SetupLCDReadCallbacks() 
// {
//     // DISPCNT — readable, static
//     // No callback needed (direct storage)

//     // DISPSTAT — readable, contains live flags (VBlank/HBlank/VCount)
//     lcdRegisters.DISPSTAT.onRead =
//         [this](u32 address) -> u32
//         {
//             // TODO: Replace with real PPU state
//             // Bits 0-2 are status flags, rest is writable configuration
//             return lcdRegisters.DISPSTAT.value;
//         };

//     // VCOUNT — live scanline counter
//     lcdRegisters.VCOUNT.onRead =
//         [this](u32 address) -> u32
//         {
//             // TODO: Hook to PPU current scanline
//             return lcdRegisters.VCOUNT.value;
//         };

//     // BGxCNT — readable, static
//     // No callback needed

//     // BGxHOFS / BGxVOFS — readable, static
//     // No callback needed

//     // BGxPA/B/C/D — readable, affine params
//     // No callback needed

//     // BGxX / BGxY — readable, latched affine reference
//     // Some emulators return internal latched value here
//     // For now, stored value is acceptable

//     // WININ / WINOUT — readable
//     // No callback needed

//     // MOSAIC — readable
//     // No callback needed

//     // BLDCNT / BLDALPHA / BLDY — readable
//     // No callback needed
// }

// void GBA_IO::SetupLCDWriteCallbacks() 
// {
//     // DISPCNT — controls display
//     lcdRegisters.DISPCNT.onWrite =
//         [this](u32 address, u32 value)
//         {
//             // TODO: Update PPU display control
//             printf("DISPCNT changed to: %u\n", value);
//             // ppu.SetDisplayControl(value);
//         };

//     // GREENSWAP — undocumented green swap
//     lcdRegisters.GREENSWAP.onWrite =
//         [this](u32 address, u32 value)
//         {
//             // TODO: Apply green swap effect in PPU
//             // ppu.SetGreenSwap(value);
//         };

//     // DISPSTAT — interrupt and scanline control
//     lcdRegisters.DISPSTAT.onWrite =
//         [this](u32 address, u32 value)
//         {
//             // TODO: Update HBlank/VBlank/LYC interrupt enables
//             // ppu.SetDispStatInterrupts(value);
//         };

//     // VCOUNT — read-only in hardware, write ignored
//     lcdRegisters.VCOUNT.onWrite =
//         [](u32, u32) { /* ignored */ };

//     // BGxCNT — background control
//     lcdRegisters.BG0CNT.onWrite =
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetBGControl(0, value);
//         };
//     lcdRegisters.BG1CNT.onWrite =
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetBGControl(1, value);
//         };
//     lcdRegisters.BG2CNT.onWrite =
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetBGControl(2, value);
//         };
//     lcdRegisters.BG3CNT.onWrite =
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetBGControl(3, value);
//         };

//     // BGxHOFS / BGxVOFS — background offsets
//     lcdRegisters.BG0HOFS.onWrite = 
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetBGOffset(0, value, lcdRegisters.BG0VOFS.value);
//         };
//     lcdRegisters.BG0VOFS.onWrite = 
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetBGOffset(0, lcdRegisters.BG0HOFS.value, value);
//         };

//     // …and similarly for BG1HOFS/VOFS, BG2HOFS/VOFS, BG3HOFS/VOFS

//     // WIN0H / WIN1H / WIN0V / WIN1V — window dimensions
//     lcdRegisters.WIN0H.onWrite =
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetWindow0H(value);
//         };

//     // BLDCNT / BLDALPHA / BLDY — blending control
//     lcdRegisters.BLDCNT.onWrite =
//         [this](u32, u32 value)
//         {
//             // TODO: ppu.SetBlending(value);
//         };
// }

// void GBA_Memory::ResetSIORegisters()
// {
//     Write16(0x04000128, 0x8000); // RCNT, uses 0x8000 as reset value
//     Clear16(0x0400012A); // JOYCNT
//     Clear32(0x04000130); // JOY_RECV
//     Clear32(0x04000134); // JOY_TRANS
//     Clear32(0x04000138); // JOYSTAT
// }

// void GBA_Memory::ResetSoundRegisters()
// {
//     Clear16(0x04000060); // SOUND1CNT_L
//     Clear16(0x04000062); // SOUND1CNT_H
//     Clear32(0x04000064); // SOUND1CNT_X

//     Clear32(0x04000068); // SOUND2CNT_L
//     Clear32(0x0400006C); // SOUND2CNT_H

//     Clear16(0x04000070); // SOUND3CNT_L
//     Clear16(0x04000072); // SOUND3CNT_H
//     Clear32(0x04000074); // SOUND3CNT_X

//     Clear32(0x04000078); // SOUND4CNT_L
//     Clear32(0x0400007C); // SOUND4CNT_H

//     Clear16(0x04000080); // SOUNDCNT_L
//     Clear16(0x04000082); // SOUNDCNT_H
//     Clear32(0x04000084); // SOUNDCNT_X

//     Write16(0x04000088, 0x0200); // SOUNDBIAS

//     Clear32(0x040000A0); // FIFO_A_L
//     Clear32(0x040000A4); // FIFO_B_L
// }

// void GBA_Memory::ResetOtherIORegisters()
// {
//     // --- Display ---
//     Write16(0x04000000, 0x0080);                // DISPCNT: forced blank enabled
//     Clear16(0x04000004);                        // DISPSTAT
    
//     // skip VCOUNT (0x04000006) – read-only
//     ClearAddressRange(0x04000008, 0x04000034);  // BGxCNT/HOFS/VOFS, WIN, MOSAIC, BLDCNT, BLDALPHA, BLDY

//     // --- DMA 0–3 ---
//     ClearAddressRange(0x040000B0, 0x040000DE);

//     // --- Timers 0–3 ---
//     ClearAddressRange(0x04000100, 0x0400010E);

//     // --- Keypad ---
//     Clear16(0x04000130);                        // KEYCNT

//     // --- Interrupts & system control ---
//     ClearAddressRange(0x04000200, 0x04000208);  // IE, IF, WAITCNT, IME
// }
