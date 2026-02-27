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
    explicit GBA_HLE(EmulatorCore* core, GBA_Memory& memory, GBA_IO& io, GBA_CPU& cpu);

    void HandleSWI(uint8_t swiNumber);

private:
    EmulatorCore* core;
    GBA_Memory& memory;
    GBA_IO& io;
    GBA_CPU& cpu;

    using SWIFunction = void(GBA_HLE::*)();
    std::array<SWIFunction, 0x43> swiTable; // 0x00 to 0x2B

    // HLE implementations:

    // Basic Functions:
    void HLE_SoftReset();                        // 0x00        
    void HLE_RegisterRamReset();                 // 0x01
    void HLE_Halt();                             // 0x02
    void HLE_Sleep();                            // 0x03
    void HLE_IntrWait();                         // 0x04
    void HLE_VBlankIntrWait();                   // 0x05
    void HLE_Div();                              // 0x06
    void HLE_DivArm();                           // 0x07
    void HLE_Sqrt();                             // 0x08
    void HLE_ArcTan();                           // 0x09
    void HLE_ArcTan2();                          // 0x0A
    void HLE_CpuSet();                           // 0x0B
    void HLE_CpuFastSet();                       // 0x0C
    void HLE_GetBiosChecksum();                  // 0x0D
    void HLE_BgAffineSet();                      // 0x0E
    void HLE_ObjAffineSet();                     // 0x0F

    // Decompression Functions
    void HLE_BitUnpack();                        // 0x10
    void HLE_LZ77UnCompReadNormalWrite8Bit();    // 0x11
    void HLE_LZ77UnCompReadNormalWrite16Bit();   // 0x12
    void HLE_HuffUnCompReadNormal();             // 0x13
    void HLE_RLUnCompReadNormalWrite8bit();      // 0x14
    void HLE_RLUnCompReadNormalWrite16bit();     // 0x15
    void HLE_Diff8bitUnFilterWrite8bit();        // 0x16
    void HLE_Diff8bitUnFilterWrite16bit();       // 0x17
    void HLE_Diff16bitUnFilter();                // 0x18

    // Sound / misc
    void HLE_SoundBias();                        // 0x19
    void HLE_SoundDriverInit();                  // 0x1A
    void HLE_SoundDriverMode();                  // 0x1B
    void HLE_SoundDriverMain();                  // 0x1C
    void HLE_SoundDriverVSync();                 // 0x1D
    void HLE_SoundChannelClear();                // 0x1E
    void HLE_MidiKey2Freq();                     // 0x1F
    void HLE_SoundWhatever0();                   // 0x20
    void HLE_SoundWhatever1();                   // 0x21
    void HLE_SoundWhatever2();                   // 0x22
    void HLE_SoundWhatever3();                   // 0x23
    void HLE_SoundWhatever4();                   // 0x24
    void HLE_MultiBoot();                        // 0x25
    void HLE_HardReset();                        // 0x26
    void HLE_CustomHalt();                       // 0x27
    void HLE_SoundDriverVSyncOff();              // 0x28
    void HLE_SoundDriverVSyncOn();               // 0x29
    void HLE_SoundGetJumpList();                 // 0x2A

    // 0x2B+ == Crash

    void BuildTable();
};