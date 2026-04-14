#include "Core/IO/GBA_IO.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/Memory/GBA_WaitstateController.hpp"
#include "Core/Memory/GBA_Memory_Helpers.hpp"
#include "Core/IO/IO_Addresses.hpp"
#include "Core/IO/TableGeneration.hpp"
#include "Core/GBA_Bus.hpp"

GBA_IO::GBA_IO(EmulatorCore* core) : core(core)
{
    ioPermissions = BuildIOPermissionTable();
}

void GBA_IO::AttachSubsystems(GBA_CPU* cpu, GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma, 
    GBA_TimerController* timers, GBA_InterruptController* interrupts, GBA_Keypad* keypad,
    GBA_WaitstateController* waitstates, GBA_Bus* bus) 
{
    this->cpu = cpu;
    this->ppu = ppu;
    this->apu = apu;
    this->dma = dma;
    this->timers = timers;
    this->interrupts = interrupts;
    this->keypad = keypad;
    this->waitstates = waitstates;
    this->bus = bus;
}

u8 GBA_IO::Read8(u32 address) 
{
    IO_LCDRegisters lcdRegs = ppu->GetLCDRegisters();

    switch (address)
    {
        // PPU
        case DISPCNT: return lcdRegs.dispcnt.Read8(0);
        case DISPCNT+1: return lcdRegs.dispcnt.Read8(1);

        case GREENSWAP: return lcdRegs.greenswap.Read8(0);
        case GREENSWAP+1: return lcdRegs.greenswap.Read8(1);

        case DISPSTAT: return lcdRegs.dispstat.Read8(0);
        case DISPSTAT+1: return lcdRegs.dispstat.Read8(1);

        case VCOUNT: return lcdRegs.vcount.Read8(0);
        case VCOUNT+1: return 0;

        case BG0CNT: return lcdRegs.bgcnt[0].Read8(0);
        case BG0CNT+1: return lcdRegs.bgcnt[0].Read8(1);
        case BG1CNT: return lcdRegs.bgcnt[1].Read8(0);
        case BG1CNT+1: return lcdRegs.bgcnt[1].Read8(1);
        case BG2CNT: return lcdRegs.bgcnt[2].Read8(0);
        case BG2CNT+1: return lcdRegs.bgcnt[2].Read8(1);
        case BG3CNT: return lcdRegs.bgcnt[3].Read8(0);
        case BG3CNT+1: return lcdRegs.bgcnt[3].Read8(1);

        case WININ: return lcdRegs.winin.Read8(0);
        case WININ+1: return lcdRegs.winin.Read8(1);

        case WINOUT: return lcdRegs.winout.Read8(0);
        case WINOUT+1: return lcdRegs.winout.Read8(1);

        case BLDCNT: return lcdRegs.bldcnt.Read8(0);
        case BLDCNT+1: return lcdRegs.bldcnt.Read8(1);

        case BLDALPHA: return lcdRegs.bldalpha.Read8(0);
        case BLDALPHA+1: return lcdRegs.bldalpha.Read8(1);
        // PPU

        // DMA

        default: return bus->OpenBus(address);
    }
}

u16 GBA_IO::Read16(u32 address)
{
    return (Read8(address + 1) << 8) | Read8(address);
}

u32 GBA_IO::Read32(u32 address) 
{
    return (Read16(address + 2) << 16) | Read16(address);
}

void GBA_IO::Write8(u32 address, u8 value) 
{
    IO_LCDRegisters lcdRegs = ppu->GetLCDRegisters();

    switch (address)
    {
        // PPU
        case DISPCNT: return lcdRegs.dispcnt.Write8(0, value);
        case DISPCNT+1: return lcdRegs.dispcnt.Write8(1, value);

        case GREENSWAP: return lcdRegs.greenswap.Write8(0, value);
        case GREENSWAP+1: return lcdRegs.greenswap.Write8(1, value);

        case DISPSTAT: return lcdRegs.dispstat.Write8(0, value);
        case DISPSTAT+1: return lcdRegs.dispstat.Write8(1, value);

        case BG0CNT: return lcdRegs.bgcnt[0].Write8(0, value);
        case BG0CNT+1: return lcdRegs.bgcnt[0].Write8(1, value);
        case BG1CNT: return lcdRegs.bgcnt[1].Write8(0, value);
        case BG1CNT+1: return lcdRegs.bgcnt[1].Write8(1, value);
        case BG2CNT: return lcdRegs.bgcnt[2].Write8(0, value);
        case BG2CNT+1: return lcdRegs.bgcnt[2].Write8(1, value);
        case BG3CNT: return lcdRegs.bgcnt[3].Write8(0, value);
        case BG3CNT+1: return lcdRegs.bgcnt[3].Write8(1, value);

        case BG0HOFS: return lcdRegs.bghofs[0].Write8(0, value);
        case BG0HOFS+1: return lcdRegs.bghofs[0].Write8(1, value);
        case BG1HOFS: return lcdRegs.bghofs[1].Write8(0, value);
        case BG1HOFS+1: return lcdRegs.bghofs[1].Write8(1, value);
        case BG2HOFS: return lcdRegs.bghofs[2].Write8(0, value);
        case BG2HOFS+1: return lcdRegs.bghofs[2].Write8(1, value);
        case BG3HOFS: return lcdRegs.bghofs[3].Write8(0, value);
        case BG3HOFS+1: return lcdRegs.bghofs[3].Write8(1, value);

        case BG2PA: return lcdRegs.bg2Params[0].Write8(0, value);
        case BG2PA+1: return lcdRegs.bg2Params[0].Write8(1, value);
        case BG2PB: return lcdRegs.bg2Params[1].Write8(0, value);
        case BG2PB+1: return lcdRegs.bg2Params[1].Write8(1, value);
        case BG2PC: return lcdRegs.bg2Params[2].Write8(0, value);
        case BG2PC+1: return lcdRegs.bg2Params[2].Write8(1, value);
        case BG2PD: return lcdRegs.bg2Params[3].Write8(0, value);
        case BG2PD+1: return lcdRegs.bg2Params[3].Write8(1, value);

        case BG2X_L: return lcdRegs.bg2XCoord.Write8(0, value);
        case BG2X_L+1: return lcdRegs.bg2XCoord.Write8(1, value);
        case BG2X_H: return lcdRegs.bg2XCoord.Write8(2, value);
        case BG2X_H+1: return lcdRegs.bg2XCoord.Write8(3, value);
        
        case BG2Y_L: return lcdRegs.bg2YCoord.Write8(0, value);
        case BG2Y_L+1: return lcdRegs.bg2YCoord.Write8(1, value);
        case BG2Y_H: return lcdRegs.bg2YCoord.Write8(2, value);
        case BG2Y_H+1: return lcdRegs.bg2YCoord.Write8(3, value);

        case BG3PA: return lcdRegs.bg3Params[0].Write8(0, value);
        case BG3PA+1: return lcdRegs.bg3Params[0].Write8(1, value);
        case BG3PB: return lcdRegs.bg3Params[1].Write8(0, value);
        case BG3PB+1: return lcdRegs.bg3Params[1].Write8(1, value);
        case BG3PC: return lcdRegs.bg3Params[2].Write8(0, value);
        case BG3PC+1: return lcdRegs.bg3Params[2].Write8(1, value);
        case BG3PD: return lcdRegs.bg3Params[3].Write8(0, value);
        case BG3PD+1: return lcdRegs.bg3Params[3].Write8(1, value);

        case BG3X_L: return lcdRegs.bg3XCoord.Write8(0, value);
        case BG3X_L+1: return lcdRegs.bg3XCoord.Write8(1, value);
        case BG3X_H: return lcdRegs.bg3XCoord.Write8(2, value);
        case BG3X_H+1: return lcdRegs.bg3XCoord.Write8(3, value);
        
        case BG3Y_L: return lcdRegs.bg3YCoord.Write8(0, value);
        case BG3Y_L+1: return lcdRegs.bg3YCoord.Write8(1, value);
        case BG3Y_H: return lcdRegs.bg3YCoord.Write8(2, value);
        case BG3Y_H+1: return lcdRegs.bg3YCoord.Write8(3, value);

        case WIN0H: return lcdRegs.winH[0].Write8(0, value);
        case WIN0H+1: return lcdRegs.winH[0].Write8(1, value);
        case WIN1H: return lcdRegs.winH[1].Write8(0, value);
        case WIN1H+1: return lcdRegs.winH[1].Write8(1, value);

        case WIN0V: return lcdRegs.winV[0].Write8(0, value);
        case WIN0V+1: return lcdRegs.winV[0].Write8(1, value);
        case WIN1V: return lcdRegs.winV[1].Write8(0, value);
        case WIN1V+1: return lcdRegs.winV[1].Write8(1, value);

        case WININ: return lcdRegs.winin.Write8(0, value);
        case WININ+1: return lcdRegs.winin.Write8(1, value);

        case WINOUT: return lcdRegs.winout.Write8(0, value);
        case WINOUT+1: return lcdRegs.winout.Write8(1, value);

        case MOSAIC: return lcdRegs.mosaic.Write8(0, value);
        case MOSAIC+1: return lcdRegs.mosaic.Write8(1, value);

        case BLDCNT: return lcdRegs.bldcnt.Write8(0, value);
        case BLDCNT+1: return lcdRegs.bldcnt.Write8(1, value);

        case BLDALPHA: return lcdRegs.bldalpha.Write8(0, value);
        case BLDALPHA+1: return lcdRegs.bldalpha.Write8(1, value);

        case BLDY: return lcdRegs.bldy.Write8(0, value);
        case BLDY+1: return lcdRegs.bldy.Write8(1, value);
        // PPU

        // DMA

        default: return;
    }
}

void GBA_IO::Write16(u32 address, u16 value) 
{
    switch (address)
    {
        default:
        {
            Write8(address + 1, (value << 8) & 0xFF);
            Write8(address, value & 0xFF);
            return;
        }
    }
}

void GBA_IO::Write32(u32 address, u32 value) 
{
    switch (address)
    {
        default:
        {
            Write16(address + 2, (value << 16) & 0xFFFF);
            Write16(address, value & 0xFFFF);
        }
    }
}

u32 GBA_IO::GetOpenBusValue(u32 address) 
{ 
    return bus->OpenBus(address); 
}

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
