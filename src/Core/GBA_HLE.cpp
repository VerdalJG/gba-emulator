#include "Core/GBA_HLE.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/GBA_CPU.hpp"
#include "Utils/Logger.hpp"

//#include <cmath>
#include <assert.h>
#include <numbers>

GBA_HLE::GBA_HLE(EmulatorCore *core) : 
core(core)
{
    assert(core != nullptr && "HLE must have valid EmulatorCore object");
    BuildTable();
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

void GBA_HLE::HLE_Halt(GBA_CPU &cpu)
{
    cpu.SetHalted(true); // CPU internally handles what to do in HandleHalt()
}

void GBA_HLE::HLE_Sleep(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_IntrWait(GBA_CPU &cpu)
{
    // High
}

void GBA_HLE::HLE_VBlankIntrWait(GBA_CPU &cpu)
{
    // ExHigh
}

void GBA_HLE::HLE_Div(GBA_CPU &cpu)
{
    int32_t dividend = cpu.GetValueAtRegister(0);
    int32_t divisor = cpu.GetValueAtRegister(1);

    int32_t quotient = dividend / divisor;
    int32_t remainder = dividend % divisor;

    cpu.SetValueAtRegister(0, quotient);
    cpu.SetValueAtRegister(1, remainder);
    cpu.SetValueAtRegister(3, abs(quotient));
}

void GBA_HLE::HLE_DivArm(GBA_CPU &cpu)
{
    int32_t dividend = cpu.GetValueAtRegister(1);
    int32_t divisor = cpu.GetValueAtRegister(0);

    int32_t quotient = dividend / divisor;
    int32_t remainder = dividend % divisor;

    cpu.SetValueAtRegister(1, quotient);
    cpu.SetValueAtRegister(0, remainder);
    cpu.SetValueAtRegister(3, abs(quotient));
}

void GBA_HLE::HLE_Sqrt(GBA_CPU &cpu)
{
    uint32_t value = cpu.GetValueAtRegister(0);

    // Sqrt of 0 is 0, don't need to continue
    if (value == 0) return;

    // Calculate highest bit so we can figure out how much we need to shift
    uint32_t highestBitIndex = 0;  
    while (value >>= 1)
    {
        ++highestBitIndex;
    }

    // Shift to the left so that when we get the sqrt, we have the decimal
    // values across the 16 bits
    value <<= (31 - highestBitIndex);
    uint16_t result = std::sqrt(value);

    // Mask result to 16 bits
    cpu.SetValueAtRegister(0, result & 0xFFFF);
}

void GBA_HLE::HLE_ArcTan(GBA_CPU &cpu)
{
    int32_t value = static_cast<int16_t>(cpu.GetValueAtRegister(0));
    double x = static_cast<double>(value) / (1 << 14); // Convert from 1.14 fixed point to float/double

    double angleRadians = std::atan(value);

    // Convert radians to GBA angle representation
    uint16_t angleGBA = static_cast<uint16_t>(angleRadians * (0x10000 / (2 * std::numbers::pi)));

    cpu.SetValueAtRegister(0, angleGBA);
}

void GBA_HLE::HLE_ArcTan2(GBA_CPU &cpu)
{
    int32_t r0 = static_cast<int16_t>(cpu.GetValueAtRegister(0));
    int32_t r1 = static_cast<int16_t>(cpu.GetValueAtRegister(1));

    double r0Angle = static_cast<int16_t>(cpu.GetValueAtRegister(0)) / (1 << 14);
    double r1Angle = static_cast<int16_t>(cpu.GetValueAtRegister(1)) / (1 << 14);

    double angleRadians = std::atan2(r1Angle, r0Angle);

    if (angleRadians < 0)
    {
        angleRadians += 2 * std::numbers::pi; // Wrapping negative angles;
    }

    // Convert to gba angle representation
    uint16_t angleGBA = static_cast<uint16_t>(angleRadians * 0x10000 / (2 * std::numbers::pi));
    cpu.SetValueAtRegister(0, angleGBA);  
}

void GBA_HLE::HLE_CpuSet(GBA_CPU &cpu)
{
    // ExHigh
    
}

void GBA_HLE::HLE_CpuFastSet(GBA_CPU &cpu)
{
    // ExHigh
}

void GBA_HLE::HLE_GetBiosChecksum(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_BgAffineSet(GBA_CPU &cpu)
{
    // Medium
}

void GBA_HLE::HLE_ObjAffineSet(GBA_CPU &cpu)
{
    // Medium
}

void GBA_HLE::HLE_BitUnpack(GBA_CPU &cpu)
{
    // Medium
}

void GBA_HLE::HLE_LZ77UnCompReadNormalWrite8Bit(GBA_CPU &cpu)
{
    // High
}

void GBA_HLE::HLE_LZ77UnCompReadNormalWrite16Bit(GBA_CPU &cpu)
{
    // High
}

void GBA_HLE::HLE_HuffUnCompReadNormal(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_RLUnCompReadNormalWrite8bit(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_RLUnCompReadNormalWrite16bit(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_Diff8bitUnFilterWrite8bit(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_Diff8bitUnFilterWrite16bit(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_Diff16bitUnFilter(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundBias(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundDriverInit(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundDriverMode(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundDriverMain(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundDriverVSync(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundChannelClear(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_MidiKey2Freq(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever0(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever1(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever2(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever3(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever4(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_MultiBoot(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_HardReset(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_CustomHalt(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundDriverVSyncOff(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundDriverVSyncOn(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::HLE_SoundGetJumpList(GBA_CPU &cpu)
{
    // Low
}

void GBA_HLE::BuildTable()
{
    swiTable = {
        &HLE_SoftReset,                         // 0x00
        &HLE_RegisterRamReset,                  // 0x01
        &HLE_Halt,                              // 0x02
        &HLE_Sleep,                             // 0x03
        &HLE_IntrWait,                          // 0x04
        &HLE_VBlankIntrWait,                    // 0x05
        &HLE_Div,                               // 0x06
        &HLE_DivArm,                            // 0x07
        &HLE_Sqrt,                              // 0x08
        &HLE_ArcTan,                            // 0x09
        &HLE_ArcTan2,                           // 0x0A
        &HLE_CpuSet,                            // 0x0B
        &HLE_CpuFastSet,                        // 0x0C
        &HLE_GetBiosChecksum,                   // 0x0D
        &HLE_BgAffineSet,                       // 0x0E
        &HLE_ObjAffineSet,                      // 0x0F
        &HLE_BitUnpack,                         // 0x10
        &HLE_LZ77UnCompReadNormalWrite8Bit,     // 0x11
        &HLE_LZ77UnCompReadNormalWrite16Bit,    // 0x12
        &HLE_HuffUnCompReadNormal,              // 0x13
        &HLE_RLUnCompReadNormalWrite8bit,       // 0x14
        &HLE_RLUnCompReadNormalWrite16bit,      // 0x15
        &HLE_Diff8bitUnFilterWrite8bit,         // 0x16
        &HLE_Diff8bitUnFilterWrite16bit,        // 0x17
        &HLE_Diff16bitUnFilter,                 // 0x18
        &HLE_SoundBias,                         // 0x19
        &HLE_SoundDriverInit,                   // 0x1A
        &HLE_SoundDriverMode,                   // 0x1B
        &HLE_SoundDriverMain,                   // 0x1C
        &HLE_SoundDriverVSync,                  // 0x1D
        &HLE_SoundChannelClear,                 // 0x1E
        &HLE_MidiKey2Freq,                      // 0x1F
        &HLE_SoundWhatever0,                    // 0x20
        &HLE_SoundWhatever1,                    // 0x21
        &HLE_SoundWhatever2,                    // 0x22
        &HLE_SoundWhatever3,                    // 0x23
        &HLE_SoundWhatever4,                    // 0x24
        &HLE_MultiBoot,                         // 0x25
        &HLE_HardReset,                         // 0x26
        &HLE_CustomHalt,                        // 0x27
        &HLE_SoundDriverVSyncOff,               // 0x28
        &HLE_SoundDriverVSyncOn,                // 0x29
        &HLE_SoundGetJumpList                   // 0x2A
    };
}
