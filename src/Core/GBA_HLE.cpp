#include "Core/GBA_HLE.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/Memory/GBA_Memory.hpp"
#include "Core/IO/GBA_IO.hpp"
#include "Core/GBA_CPU.hpp"

#include "Utils/Logger.hpp"

#include <assert.h>
#include <numbers>

GBA_HLE::GBA_HLE(EmulatorCore* core, GBA_Memory& memory, GBA_IO& io, GBA_CPU& cpu) : 
    core(core), memory(memory), io(io), cpu(cpu)
{
    assert(core != nullptr && "HLE must have valid EmulatorCore object");
    BuildTable();
}

void GBA_HLE::HandleSWI(uint8_t swiNumber)
{
    if (swiNumber >= swiTable.size())
    {
        core->PostStatus("Unknown SWI: " + swiNumber);
        return;
    }

    auto& swiFunc = swiTable[swiNumber];
    if (swiFunc)
    {
        (this->*swiFunc)();;
    }
    else
    {
        core->PostStatus("Unimplemented SWI: " + swiNumber);
    }
}

void GBA_HLE::HLE_SoftReset()
{
    // Reset registers to startup state
    for (int i = 0; i < 16; ++i)
    {
        cpu.WriteRegister(i, 0);
    }
        
    // Reset CPSR (enter system mode, ARM state)
    cpu.SetCPSR(0x000000DF); // I and F disabled, ARM, System mode

    // Restart at address 0 (BIOS entry point)
    cpu.WriteRegister(15, 0x00000000);
}

void GBA_HLE::HLE_RegisterRamReset()
{
    uint32_t mask = cpu.ReadRegister(0);

    if (mask & (1 << 0)) memory.ClearRegion(RegionType::EWRAM);
    if (mask & (1 << 1)) memory.ClearRegion(RegionType::IWRAM);
    if (mask & (1 << 2)) memory.ClearRegion(RegionType::PaletteRAM);
    if (mask & (1 << 3)) memory.ClearRegion(RegionType::VRAM);
    if (mask & (1 << 4)) memory.ClearRegion(RegionType::OAM);

    // These only clear some specific parts of the memory regions
    // if (mask & (1 << 5)) io.ResetSIORegisters();
    // if (mask & (1 << 6)) io.ResetSoundRegisters();
    // if (mask & (1 << 7)) io.ResetOtherIORegisters();
}

void GBA_HLE::HLE_Halt()
{
    cpu.SetHalted(true); // CPU internally handles what to do in HandleHalt()
}

void GBA_HLE::HLE_Sleep()
{
    // Low
}

void GBA_HLE::HLE_IntrWait()
{
    // High
}

void GBA_HLE::HLE_VBlankIntrWait()
{
    // ExHigh
}

void GBA_HLE::HLE_Div()
{
    int32_t dividend = cpu.ReadRegister(0);
    int32_t divisor = cpu.ReadRegister(1);

    int32_t quotient = dividend / divisor;
    int32_t remainder = dividend % divisor;

    cpu.WriteRegister(0, quotient);
    cpu.WriteRegister(1, remainder);
    cpu.WriteRegister(3, abs(quotient));
}

void GBA_HLE::HLE_DivArm()
{
    int32_t dividend = cpu.ReadRegister(1);
    int32_t divisor = cpu.ReadRegister(0);

    int32_t quotient = dividend / divisor;
    int32_t remainder = dividend % divisor;

    cpu.WriteRegister(1, quotient);
    cpu.WriteRegister(0, remainder);
    cpu.WriteRegister(3, abs(quotient));
}

void GBA_HLE::HLE_Sqrt()
{
    uint32_t value = cpu.ReadRegister(0);

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
    cpu.WriteRegister(0, result & 0xFFFF);
}

void GBA_HLE::HLE_ArcTan()
{
    int32_t value = static_cast<int16_t>(cpu.ReadRegister(0));
    double x = static_cast<double>(value) / (1 << 14); // Convert from 1.14 fixed point to float/double

    double angleRadians = std::atan(value);

    // Convert radians to GBA angle representation
    uint16_t angleGBA = static_cast<uint16_t>(angleRadians * (0x10000 / (2 * std::numbers::pi)));

    cpu.WriteRegister(0, angleGBA);
}

void GBA_HLE::HLE_ArcTan2()
{
    int32_t r0 = static_cast<int16_t>(cpu.ReadRegister(0));
    int32_t r1 = static_cast<int16_t>(cpu.ReadRegister(1));

    double r0Angle = static_cast<int16_t>(cpu.ReadRegister(0)) / (1 << 14);
    double r1Angle = static_cast<int16_t>(cpu.ReadRegister(1)) / (1 << 14);

    double angleRadians = std::atan2(r1Angle, r0Angle);

    if (angleRadians < 0)
    {
        angleRadians += 2 * std::numbers::pi; // Wrapping negative angles;
    }

    // Convert to gba angle representation
    uint16_t angleGBA = static_cast<uint16_t>(angleRadians * 0x10000 / (2 * std::numbers::pi));
    cpu.WriteRegister(0, angleGBA);  
}

void GBA_HLE::HLE_CpuSet()
{
    // ExHigh
    
}

void GBA_HLE::HLE_CpuFastSet()
{
    // ExHigh
}

void GBA_HLE::HLE_GetBiosChecksum()
{
    // Low
}

void GBA_HLE::HLE_BgAffineSet()
{
    // Medium
}

void GBA_HLE::HLE_ObjAffineSet()
{
    // Medium
}

void GBA_HLE::HLE_BitUnpack()
{
    // Medium
}

void GBA_HLE::HLE_LZ77UnCompReadNormalWrite8Bit()
{
    // High
}

void GBA_HLE::HLE_LZ77UnCompReadNormalWrite16Bit()
{
    // High
}

void GBA_HLE::HLE_HuffUnCompReadNormal()
{
    // Low
}

void GBA_HLE::HLE_RLUnCompReadNormalWrite8bit()
{
    // Low
}

void GBA_HLE::HLE_RLUnCompReadNormalWrite16bit()
{
    // Low
}

void GBA_HLE::HLE_Diff8bitUnFilterWrite8bit()
{
    // Low
}

void GBA_HLE::HLE_Diff8bitUnFilterWrite16bit()
{
    // Low
}

void GBA_HLE::HLE_Diff16bitUnFilter()
{
    // Low
}

void GBA_HLE::HLE_SoundBias()
{
    // Low
}

void GBA_HLE::HLE_SoundDriverInit()
{
    // Low
}

void GBA_HLE::HLE_SoundDriverMode()
{
    // Low
}

void GBA_HLE::HLE_SoundDriverMain()
{
    // Low
}

void GBA_HLE::HLE_SoundDriverVSync()
{
    // Low
}

void GBA_HLE::HLE_SoundChannelClear()
{
    // Low
}

void GBA_HLE::HLE_MidiKey2Freq()
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever0()
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever1()
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever2()
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever3()
{
    // Low
}

void GBA_HLE::HLE_SoundWhatever4()
{
    // Low
}

void GBA_HLE::HLE_MultiBoot()
{
    // Low
}

void GBA_HLE::HLE_HardReset()
{
    // Low
}

void GBA_HLE::HLE_CustomHalt()
{
    // Low
}

void GBA_HLE::HLE_SoundDriverVSyncOff()
{
    // Low
}

void GBA_HLE::HLE_SoundDriverVSyncOn()
{
    // Low
}

void GBA_HLE::HLE_SoundGetJumpList()
{
    // Low
}

void GBA_HLE::BuildTable()
{
    swiTable = {
        &GBA_HLE::HLE_SoftReset,                         // 0x00
        &GBA_HLE::HLE_RegisterRamReset,                  // 0x01
        &GBA_HLE::HLE_Halt,                              // 0x02
        &GBA_HLE::HLE_Sleep,                             // 0x03
        &GBA_HLE::HLE_IntrWait,                          // 0x04
        &GBA_HLE::HLE_VBlankIntrWait,                    // 0x05
        &GBA_HLE::HLE_Div,                               // 0x06
        &GBA_HLE::HLE_DivArm,                            // 0x07
        &GBA_HLE::HLE_Sqrt,                              // 0x08
        &GBA_HLE::HLE_ArcTan,                            // 0x09
        &GBA_HLE::HLE_ArcTan2,                           // 0x0A
        &GBA_HLE::HLE_CpuSet,                            // 0x0B
        &GBA_HLE::HLE_CpuFastSet,                        // 0x0C
        &GBA_HLE::HLE_GetBiosChecksum,                   // 0x0D
        &GBA_HLE::HLE_BgAffineSet,                       // 0x0E
        &GBA_HLE::HLE_ObjAffineSet,                      // 0x0F
        &GBA_HLE::HLE_BitUnpack,                         // 0x10
        &GBA_HLE::HLE_LZ77UnCompReadNormalWrite8Bit,     // 0x11
        &GBA_HLE::HLE_LZ77UnCompReadNormalWrite16Bit,    // 0x12
        &GBA_HLE::HLE_HuffUnCompReadNormal,              // 0x13
        &GBA_HLE::HLE_RLUnCompReadNormalWrite8bit,       // 0x14
        &GBA_HLE::HLE_RLUnCompReadNormalWrite16bit,      // 0x15
        &GBA_HLE::HLE_Diff8bitUnFilterWrite8bit,         // 0x16
        &GBA_HLE::HLE_Diff8bitUnFilterWrite16bit,        // 0x17
        &GBA_HLE::HLE_Diff16bitUnFilter,                 // 0x18
        &GBA_HLE::HLE_SoundBias,                         // 0x19
        &GBA_HLE::HLE_SoundDriverInit,                   // 0x1A
        &GBA_HLE::HLE_SoundDriverMode,                   // 0x1B
        &GBA_HLE::HLE_SoundDriverMain,                   // 0x1C
        &GBA_HLE::HLE_SoundDriverVSync,                  // 0x1D
        &GBA_HLE::HLE_SoundChannelClear,                 // 0x1E
        &GBA_HLE::HLE_MidiKey2Freq,                      // 0x1F
        &GBA_HLE::HLE_SoundWhatever0,                    // 0x20
        &GBA_HLE::HLE_SoundWhatever1,                    // 0x21
        &GBA_HLE::HLE_SoundWhatever2,                    // 0x22
        &GBA_HLE::HLE_SoundWhatever3,                    // 0x23
        &GBA_HLE::HLE_SoundWhatever4,                    // 0x24
        &GBA_HLE::HLE_MultiBoot,                         // 0x25
        &GBA_HLE::HLE_HardReset,                         // 0x26
        &GBA_HLE::HLE_CustomHalt,                        // 0x27
        &GBA_HLE::HLE_SoundDriverVSyncOff,               // 0x28
        &GBA_HLE::HLE_SoundDriverVSyncOn,                // 0x29
        &GBA_HLE::HLE_SoundGetJumpList                   // 0x2A
    };
}
