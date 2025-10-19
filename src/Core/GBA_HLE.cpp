#include "Core/GBA_HLE.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/CPU/CPU_Memory.hpp"
#include <assert.h>

#include "Core/CPU/GBA_CPU.hpp"
#include "Utils/Logger.hpp"

GBA_HLE::GBA_HLE(EmulatorCore *core) : 
core(core)
{
    assert(core != nullptr && "HLE must have valid EmulatorCore object");
}

void GBA_HLE::HandleSWI(uint8_t swiNumber, GBA_CPU &cpu)
{
    if (swiNumber >= swiTable.size())
    {
        core->PostStatus("Unknown SWI: " + swiNumber);
        return;
    }

    auto& swiFunc = swiTable[swiNumber];
    if (swiFunc)
    {
        swiFunc(cpu);
    }
    else
    {
        core->PostStatus("Unimplemented SWI: " + swiNumber);
    }
}

void GBA_HLE::HLE_SoftReset(GBA_CPU &cpu)
{
    // Reset registers to startup state
    for (int i = 0; i < 16; ++i)
    {
        cpu.SetValueAtRegister(i, 0);
    }
        
    // Reset CPSR (enter system mode, ARM state)
    cpu.UpdateCPSR(0x000000DF); // I and F disabled, ARM, System mode

    // Restart at address 0 (BIOS entry point)
    cpu.SetValueAtRegister(GBA_CPU::PC_INDEX, 0x00000000);
}

void GBA_HLE::HLE_RegisterRamReset(GBA_CPU &cpu)
{
    uint32_t mask = cpu.GetValueAtRegister(0);

    if (mask & (1 << 0)) cpu.GetMemorySystem().ClearRegion(RegionType::EWRAM);
    if (mask & (1 << 1)) cpu.GetMemorySystem().ClearRegion(RegionType::IWRAM);
    if (mask & (1 << 2)) cpu.GetMemorySystem().ClearRegion(RegionType::PaletteRAM);
    if (mask & (1 << 3)) cpu.GetMemorySystem().ClearRegion(RegionType::VRAM);
    if (mask & (1 << 4)) cpu.GetMemorySystem().ClearRegion(RegionType::OAM);

    // These only clear some specific parts of the memory regions
    if (mask & (1 << 5)) cpu.GetMemorySystem().ResetSIORegisters();
    if (mask & (1 << 6)) cpu.GetMemorySystem().ResetSoundRegisters();
    if (mask & (1 << 7)) cpu.GetMemorySystem().ResetOtherIORegisters();
}

void GBA_HLE::HLE_VBlankIntrWait(GBA_CPU &cpu)
{
}

void GBA_HLE::HLE_Div(GBA_CPU &cpu)
{
}

void GBA_HLE::BuildTable()
{
    swiTable = {
        &HLE_SoftReset,         // 0x00
        &HLE_RegisterRamReset,  // 0x01
        &HLE_VBlankIntrWait,    // 0x05
        &HLE_Div                // 0x06
        // Add more soon
    };
}
