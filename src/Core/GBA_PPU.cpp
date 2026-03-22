#include "Core/GBA_PPU.hpp"
#include "Core/GBA_Bus.hpp"
#include "Core/Memory/GBA_Memory_Helpers.hpp"

#include <assert.h>

GBA_PPU::GBA_PPU(EmulatorCore *core, GBA_Bus& bus) : 
    core(core), 
    bus(bus)
{
    assert(core != nullptr && "PPU must have valid EmulatorCore object");
}

uint8_t GBA_PPU::Read8_Bus(uint32_t address) 
{ 
    return bus.Read<u8>(address, BusRequester::PPU);
}

uint16_t GBA_PPU::Read16_Bus(uint32_t address) 
{ 
    return bus.Read<u16>(address, BusRequester::PPU);
}

uint32_t GBA_PPU::Read32_Bus(uint32_t address) 
{ 
    return bus.Read<u32>(address, BusRequester::PPU);
}

void GBA_PPU::RenderFrame() 
{
    /* 
    Read LCD state (DISPCNT)
    If forced blank → output black
    If not Mode 4 → do nothing (for now)
    Read active Mode 4 framebuffer from VRAM
    Read BG palette entries
    Convert indexed pixels → ARGB8888
    Write into frameBuffer[]
    */

    // Read LCD state
    frameReady = false;

    const uint16_t dispcnt = lcdRegisters.DISPCNT.value;
    
    if (dispcnt & (1 << 7)) // Check for forced blank
    {
        // Output black screen
        for (uint32_t i = 0; i < 240 * 160; ++i)
        {
            frameBuffer[i] = 0xFF000000; // opaque black (ARGB)
        }
            
        frameReady = true;
        return;
    }

    // Extract video mode (bits 0–2)
    const uint32_t mode = dispcnt & 0x7;

    // Only Mode 4 supported for now
    if (mode != 4)
    {
        // Do nothing for now
        return;
    }

    // Framebuffer page select (bit 4)
    const bool frame1 = dispcnt & (1 << 4);

    const uint32_t paletteBase = PALETTE_RAM_START; // BG palette

    const uint32_t vramBase = frame1 ? 0x0600A000 : 0x06000000;

    // Construct frame buffer (pixels)
    for (uint32_t y = 0; y < GBA_HEIGHT; y++)
    {
        for (uint32_t x = 0; x < GBA_WIDTH; x++)
        {
            const uint32_t pixelIndex = (y * GBA_WIDTH) + x;

            // Read 8-bit pixel index
            const uint8_t colorIndex = Read8_Bus(vramBase + pixelIndex);

            // Read BG palette entry (BGR555)
            const uint16_t bgr = Read16_Bus(paletteBase + colorIndex * 2);

            // Convert BGR555 -> RGB888
            const uint8_t r = (bgr & 0x1F) << 3;
            const uint8_t g = ((bgr >> 5) & 0x1F) << 3;
            const uint8_t b = ((bgr >> 10) & 0x1F) << 3;

            frameBuffer[pixelIndex] = 
                0xFF000000 | // Alpha
                (r << 16) | // Red
                (g << 8) | // Green
                b; // Blue
        }
    }

    frameReady = true;
}

bool GBA_PPU::FrameReady()
{
    if (frameReady)
    {
        frameReady = false;
        return true;
    }

    return false;
}

void GBA_PPU::Step(u32 cycles) 
{
    dotCycle += cycles;

    while (dotCycle >= 1232)
    {
        dotCycle -= 1232;
        scanline++;

        if (scanline == 160)
        {
            RenderFrame();
        }

        if (scanline == 228)
        {
            scanline = 0;
        }
    }

    // Update VCOUNT
    lcdRegisters.VCOUNT.value = scanline;

    // Update DISPSTAT flags
    u16 stat = lcdRegisters.DISPSTAT.value & ~0x7;

    if (scanline >= 160)
        stat |= 1;      // VBlank

    if (dotCycle >= 960)
        stat |= 2;      // HBlank

    if (scanline == ((lcdRegisters.DISPSTAT.value >> 8) & 0xFF))
        stat |= 4;      // VCount match

    lcdRegisters.DISPSTAT.value = stat;
}

bool GBA_PPU::IsWithinVRAM_OBJBoundary(u32 address) 
{ 
    uint mode = lcdRegisters.DISPCNT.value & 7;
    u32 vramBoundary = VRAM_START + VRAM_BG_SIZE;
    if (mode >= 3)
    {
        vramBoundary += VRAM_BOUNDARY_BITMAP_OFFSET;
    }

    return address > vramBoundary;
}
