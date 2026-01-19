#include "Core/GBA_IO.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_IO_Helpers.hpp"

GBA_IO::GBA_IO(EmulatorCore* core, GBA_PPU* ppu, GBA_APU* apu, 
    GBA_DMAController* dma, GBA_TimerController* timers, 
    GBA_InterruptController* interrupts) : core(core), ppu(ppu), apu(apu), 
    dma(dma), timers(timers), 
    interrupts(interrupts)
{}

uint8_t GBA_IO::Read8(uint32_t address)
{
    uint16_t halfword = Read16(address & ~1u);
    return (address & 1) ? (halfword >> 8) : (halfword & 0xFF);
}

uint16_t GBA_IO::Read16(uint32_t address)
{
    if (address < LCD_END)
        return ppu->Read16(address);

    if (address < SOUND_END)
        //return apu->Read16(address);

    if (address < DMA_END)
        //return dma->Read16(address);

    if (address < TIMER_END)
        //return timers->Read16(address);

    if (address < SIO_END)
        //return sio->Read16(address);

    if (address < KEYPAD_END)
        //return keypad->Read16(address);   
    
    if (address < IRQ_END)
        //return interrupt->Read16(address);   

    return 0; // For now, should return open bus 
}

uint32_t GBA_IO::Read32(uint32_t address) 
{
    address &= ~3u;
    uint16_t low = Read16(address);
    uint16_t high = Read16(address + 2);
    return static_cast<uint32_t>(low) | (static_cast<uint32_t>(high) << 16);
}

void GBA_IO::Write8(uint32_t address, uint8_t value) 
{
    
}

void GBA_IO::Write16(uint32_t address, uint16_t value) 
{
    if (address < LCD_END)
        return ppu->Write16(address, value);

    if (address < SOUND_END)
        //return apu->Write16(address, value);

    if (address < DMA_END)
        //return dma->Write16(address, value);

    if (address < TIMER_END)
        //return timers->Write16(address, value);

    if (address < SIO_END)
        //return sio->Write16(address, value);

    if (address < KEYPAD_END)
        //return keypad->Write16(address, value);  
    
    if (address < IRQ_END)
        //return interrupt->Write16(address, value);
    
    return; // for compilation error
}

void GBA_IO::Write32(uint32_t address, uint32_t value) 
{

}

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
