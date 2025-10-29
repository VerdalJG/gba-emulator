#pragma once
#include <cstdint>
#include <functional>
#include <array>
#include <string>


class EmulatorCore;
class GBA_CPU;

class GBA_HLE
{
public:
    explicit GBA_HLE(EmulatorCore* core);

    void HandleSWI(uint8_t swiNumber, GBA_CPU& cpu);

private:
    EmulatorCore* core;

    using SWIFunction = std::function<void(GBA_CPU& cpu)>;
    std::array<SWIFunction, 0x43> swiTable; // 0x00 to 0x2B

    // HLE implementations:

    // Basic Functions:
    static void HLE_SoftReset(GBA_CPU& cpu);                        // 0x00        
    static void HLE_RegisterRamReset(GBA_CPU& cpu);                 // 0x01
    static void HLE_Halt(GBA_CPU& cpu);                             // 0x02
    static void HLE_Sleep(GBA_CPU& cpu);                            // 0x03
    static void HLE_IntrWait(GBA_CPU& cpu);                         // 0x04
    static void HLE_VBlankIntrWait(GBA_CPU& cpu);                   // 0x05
    static void HLE_Div(GBA_CPU& cpu);                              // 0x06
    static void HLE_DivArm(GBA_CPU& cpu);                           // 0x07
    static void HLE_Sqrt(GBA_CPU& cpu);                             // 0x08
    static void HLE_ArcTan(GBA_CPU& cpu);                           // 0x09
    static void HLE_ArcTan2(GBA_CPU& cpu);                          // 0x0A
    static void HLE_CpuSet(GBA_CPU& cpu);                           // 0x0B
    static void HLE_CpuFastSet(GBA_CPU& cpu);                       // 0x0C
    static void HLE_GetBiosChecksum(GBA_CPU& cpu);                  // 0x0D
    static void HLE_BgAffineSet(GBA_CPU& cpu);                      // 0x0E
    static void HLE_ObjAffineSet(GBA_CPU& cpu);                     // 0x0F

    // Decompression Functions
    static void HLE_BitUnpack(GBA_CPU& cpu);                        // 0x10
    static void HLE_LZ77UnCompReadNormalWrite8Bit(GBA_CPU& cpu);    // 0x11
    static void HLE_LZ77UnCompReadNormalWrite16Bit(GBA_CPU& cpu);   // 0x12
    static void HLE_HuffUnCompReadNormal(GBA_CPU& cpu);             // 0x13
    static void HLE_RLUnCompReadNormalWrite8bit(GBA_CPU& cpu);      // 0x14
    static void HLE_RLUnCompReadNormalWrite16bit(GBA_CPU& cpu);     // 0x15
    static void HLE_Diff8bitUnFilterWrite8bit(GBA_CPU& cpu);        // 0x16
    static void HLE_Diff8bitUnFilterWrite16bit(GBA_CPU& cpu);       // 0x17
    static void HLE_Diff16bitUnFilter(GBA_CPU& cpu);                // 0x18

    // Sound (and Multiboot/HardReset/CustomHalt)
    static void HLE_SoundBias(GBA_CPU& cpu);                        // 0x19
    static void HLE_SoundDriverInit(GBA_CPU& cpu);                  // 0x1A
    static void HLE_SoundDriverMode(GBA_CPU& cpu);                  // 0x1B
    static void HLE_SoundDriverMain(GBA_CPU& cpu);                  // 0x1C
    static void HLE_SoundDriverVSync(GBA_CPU& cpu);                 // 0x1D
    static void HLE_SoundChannelClear(GBA_CPU& cpu);                // 0x1E
    static void HLE_MidiKey2Freq(GBA_CPU& cpu);                     // 0x1F
    static void HLE_SoundWhatever0(GBA_CPU& cpu);                   // 0x20
    static void HLE_SoundWhatever1(GBA_CPU& cpu);                   // 0x21
    static void HLE_SoundWhatever2(GBA_CPU& cpu);                   // 0x22
    static void HLE_SoundWhatever3(GBA_CPU& cpu);                   // 0x23
    static void HLE_SoundWhatever4(GBA_CPU& cpu);                   // 0x24
    static void HLE_MultiBoot(GBA_CPU& cpu);                        // 0x25
    static void HLE_HardReset(GBA_CPU& cpu);                        // 0x26
    static void HLE_CustomHalt(GBA_CPU& cpu);                       // 0x27
    static void HLE_SoundDriverVSyncOff(GBA_CPU& cpu);              // 0x28
    static void HLE_SoundDriverVSyncOn(GBA_CPU& cpu);               // 0x29
    static void HLE_SoundGetJumpList(GBA_CPU& cpu);                 // 0x2A

    // 0x2B+ == Crash

    void BuildTable();
};