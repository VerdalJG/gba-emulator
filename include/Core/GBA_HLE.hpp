#pragma once
#include <cstdint>
#include <functional>
#include <array>
#include <string>


class EmulatorCore;
class GBA_Memory;
class GBA_IO;
class GBA_CPU;

class GBA_HLE
{
public:
    explicit GBA_HLE(EmulatorCore* core, GBA_Memory& memory, GBA_IO& io);

    void HandleSWI(uint8_t swiNumber, GBA_CPU& cpu);

private:
    EmulatorCore* core;
    GBA_Memory& memory;
    GBA_IO& io;

    using SWIFunction = void(GBA_HLE::*)(GBA_CPU&);
    std::array<SWIFunction, 0x43> swiTable; // 0x00 to 0x2B

    // HLE implementations:

    // Basic Functions:
    void HLE_SoftReset(GBA_CPU& cpu);                        // 0x00        
    void HLE_RegisterRamReset(GBA_CPU& cpu);                 // 0x01
    void HLE_Halt(GBA_CPU& cpu);                             // 0x02
    void HLE_Sleep(GBA_CPU& cpu);                            // 0x03
    void HLE_IntrWait(GBA_CPU& cpu);                         // 0x04
    void HLE_VBlankIntrWait(GBA_CPU& cpu);                   // 0x05
    void HLE_Div(GBA_CPU& cpu);                              // 0x06
    void HLE_DivArm(GBA_CPU& cpu);                           // 0x07
    void HLE_Sqrt(GBA_CPU& cpu);                             // 0x08
    void HLE_ArcTan(GBA_CPU& cpu);                           // 0x09
    void HLE_ArcTan2(GBA_CPU& cpu);                          // 0x0A
    void HLE_CpuSet(GBA_CPU& cpu);                           // 0x0B
    void HLE_CpuFastSet(GBA_CPU& cpu);                       // 0x0C
    void HLE_GetBiosChecksum(GBA_CPU& cpu);                  // 0x0D
    void HLE_BgAffineSet(GBA_CPU& cpu);                      // 0x0E
    void HLE_ObjAffineSet(GBA_CPU& cpu);                     // 0x0F

    // Decompression Functions
    void HLE_BitUnpack(GBA_CPU& cpu);                        // 0x10
    void HLE_LZ77UnCompReadNormalWrite8Bit(GBA_CPU& cpu);    // 0x11
    void HLE_LZ77UnCompReadNormalWrite16Bit(GBA_CPU& cpu);   // 0x12
    void HLE_HuffUnCompReadNormal(GBA_CPU& cpu);             // 0x13
    void HLE_RLUnCompReadNormalWrite8bit(GBA_CPU& cpu);      // 0x14
    void HLE_RLUnCompReadNormalWrite16bit(GBA_CPU& cpu);     // 0x15
    void HLE_Diff8bitUnFilterWrite8bit(GBA_CPU& cpu);        // 0x16
    void HLE_Diff8bitUnFilterWrite16bit(GBA_CPU& cpu);       // 0x17
    void HLE_Diff16bitUnFilter(GBA_CPU& cpu);                // 0x18

    // Sound / misc
    void HLE_SoundBias(GBA_CPU& cpu);                        // 0x19
    void HLE_SoundDriverInit(GBA_CPU& cpu);                  // 0x1A
    void HLE_SoundDriverMode(GBA_CPU& cpu);                  // 0x1B
    void HLE_SoundDriverMain(GBA_CPU& cpu);                  // 0x1C
    void HLE_SoundDriverVSync(GBA_CPU& cpu);                 // 0x1D
    void HLE_SoundChannelClear(GBA_CPU& cpu);                // 0x1E
    void HLE_MidiKey2Freq(GBA_CPU& cpu);                     // 0x1F
    void HLE_SoundWhatever0(GBA_CPU& cpu);                   // 0x20
    void HLE_SoundWhatever1(GBA_CPU& cpu);                   // 0x21
    void HLE_SoundWhatever2(GBA_CPU& cpu);                   // 0x22
    void HLE_SoundWhatever3(GBA_CPU& cpu);                   // 0x23
    void HLE_SoundWhatever4(GBA_CPU& cpu);                   // 0x24
    void HLE_MultiBoot(GBA_CPU& cpu);                        // 0x25
    void HLE_HardReset(GBA_CPU& cpu);                        // 0x26
    void HLE_CustomHalt(GBA_CPU& cpu);                       // 0x27
    void HLE_SoundDriverVSyncOff(GBA_CPU& cpu);              // 0x28
    void HLE_SoundDriverVSyncOn(GBA_CPU& cpu);               // 0x29
    void HLE_SoundGetJumpList(GBA_CPU& cpu);                 // 0x2A

    // 0x2B+ == Crash

    void BuildTable();
};