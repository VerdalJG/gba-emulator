#include "Core/GBA_PPU.hpp"
#include "Core/GBA_Bus.hpp"
#include "Core/GBA_Memory_Helpers.hpp"

#include <assert.h>

GBA_PPU::GBA_PPU(EmulatorCore *core, GBA_Bus& bus, IO_LCDRegisters& lcdRegisters) : 
    core(core), 
    bus(bus),
    lcdRegisters(lcdRegisters)
{
    assert(core != nullptr && "PPU must have valid EmulatorCore object");
}

uint8_t GBA_PPU::Read8_Bus(uint32_t address) 
{ 
    return bus.Read8(address, BusRequester::PPU, nullptr);
}

uint16_t GBA_PPU::Read16_Bus(uint32_t address) 
{ 
    return bus.Read16(address, BusRequester::PPU, nullptr);
}

uint32_t GBA_PPU::Read32_Bus(uint32_t address) 
{ 
    return bus.Read32(address, BusRequester::PPU, nullptr);
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

            if (pixelIndex < 10)
            {
                printf("VRAM[%u] = %u\n", pixelIndex, colorIndex);
                printf("Palette[0] = %04X\n", Read16_Bus(0x05000000));
            } 
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

void GBA_PPU::Step(uint32_t cycles) 
{
    frameCycleCounter += cycles;

    if (frameCycleCounter >= CYCLES_PER_FRAME)
    {
        frameCycleCounter -= CYCLES_PER_FRAME;
        RenderFrame();
        frameReady = true;
    }
}

