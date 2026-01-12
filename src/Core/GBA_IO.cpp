#include "Core/GBA_IO.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_IO_Helpers.hpp"

GBA_IO::GBA_IO(EmulatorCore* core, GBA_PPU* ppu, GBA_APU* apu, 
    GBA_DMAController* dma, GBA_TimerController* timers, 
    GBA_InterruptController* interrupt) : core(core), ppu(ppu), apu(apu), 
    dma(dma), timers(timers), 
    interrupt(interrupt)
{
    
}

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

    return core->GetMemory().GetLastBusValue<uint16_t>();
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
