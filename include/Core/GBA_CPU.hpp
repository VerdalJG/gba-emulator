#pragma once

#include <cstdint>
#include <array>
#include "Core/CPU/CPU_Modes.hpp"
#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"
#include "Utils/Logger.hpp"
#include "Utils/Integers.hpp"
#include "Core/CPU/Registers.hpp"
#include "Core/CPU/Instructions/TableGenerator.hpp"
#include "Core/GBA_Bus.hpp"

#include "Core/CPU/InstructionPipeline.hpp"

class GBA_Bus;
class EmulatorCore;

// Emulates the ARM7TDMI, ARMv4t core
class GBA_CPU 
{
public:
    GBA_CPU(EmulatorCore* core, GBA_Bus& bus);
    ~GBA_CPU() = default;

    void Reset();            // Reset to CPU initial state
    void Step();             // Fetch, decode, and execute loop
    void RequestInterrupt(); // Triggered by emulator core

    // Register functions
    inline uint32_t ReadRegister(int index) { return cpuState.registers[index]; }
    
    inline uint32_t GetValueAtUserRegister(int registerIndex) { return sharedR8_R12[8 - registerIndex]; }
    inline void SetValueAtUserRegister(int registerIndex, uint32_t value) { sharedR8_R12[8 - registerIndex] = value; }
    inline void AdvanceProgramCounter() { cpuState.r15 += (IsThumbMode() ? 2u : 4u); }

    // CPSR functions
    inline uint GetCPSR() { return cpuState.cpsr.value; }
    inline uint GetCPSR_N() { return cpuState.cpsr.fields.n; }
    inline uint GetCPSR_Z() { return cpuState.cpsr.fields.z; }
    inline uint GetCPSR_C() { return cpuState.cpsr.fields.c; }
    inline uint GetCPSR_V() { return cpuState.cpsr.fields.v; }
    inline bool IsThumbMode() { return cpuState.cpsr.fields.thumb; }
    
    inline Mode GetCurrentMode() { return static_cast<Mode>(cpuState.cpsr.fields.mode); }
    inline void UpdateCPSR(uint32_t bits, uint32_t bitsToUpdate = 0xFFFFFFFF) { cpsr = (cpsr & ~bitsToUpdate) | bits; }
    
    // SPSR functions
    inline bool CurrentModeHasSPSR() { return (GetCurrentMode() != Mode::USR) && (GetCurrentMode() != Mode::SYS); } 
    
    void RestoreCPSRFromSPSR(int oldExceptionModeIndex);
    void SaveCPSRIntoSPSR(int exceptionModeIndex);
    void UpdateVisibleRegistersForMode(OperatingMode newMode);

    void SwitchMode(Mode mode);

    bool GetHalted() { return halted; }
    void SetHalted(bool shouldHalt) { halted = shouldHalt; }

    void AddCycles(uint32_t cycles);
    uint32_t GetTotalCycles() { return totalCycles; }
    uint32_t GetCurrentInstructionCycles() { return currentInstructionCycles; }
    uint32_t GetCurrentFrameCycles() { return currentFrameCycles; }

    EmulatorCore* GetEmulatorCore();
    void Log(const std::string& message, LogType logType, const char* functionName = nullptr);

    u32 Read8(u32 address, uint access);
    u32 Read16(u32 address, uint access);
    u32 Read32(u32 address, uint access);

    void Write8(u32 address, u8 value, uint access);
    void Write16(u32 address, u16 value, uint access);
    void Write32(u32 address, u32 value, uint access);

    // Used for LDR and SWP
    u32 Read16_Rotated(u32 address, uint access);
    u32 Read32_Rotated(u32 address, uint access);

    u32 Read8_SignExtended(u32 address, uint access);
    u32 Read16_SignExtended(u32 address, uint access);

    void InvalidateSequentiality(); 

    using Handler_ARM = void (GBA_CPU::*)(u32); 
    using Handler_Thumb = void (GBA_CPU::*)(u16);

    // Thumb instructions:
    template <u16 shiftOp, u16 immediate_5>
    void Thumb_MoveShiftedRegister(u16 instruction);

    template <bool subtract, bool immediate, u16 operand>
    void Thumb_AddSubtract(u16 instruction);

    template <u16 opcode, u16 rdIndex>
    void Thumb_ImmediateOp(u16 instruction);

    template <u16 opcode>
    void Thumb_ALU(u16 instruction);

    template <u16 opcode, u16 msbRd, u16 msbRs>
    void Thumb_HiRegisterOp(u16 instruction);

    template <u16 rdIndex>
    void Thumb_LoadPCRelative(u16 instruction);

    template <u16 opcode, u16 roIndex>
    void Thumb_LoadStoreRegisterOffset(u16 instruction);

    template <u16 opcode, u16 roIndex>
    void Thumb_LoadStoreSignExtended(u16 instruction);

    template <u16 opcode, u16 offset_5>
    void Thumb_LoadStoreImmediateOffset(u16 instruction);

    template <bool load, u16 offset_5>
    void Thumb_LoadStoreHalfword(u16 instruction);

    template <bool load, u16 rdIndex>
    void Thumb_LoadStoreSPRelative(u16 instruction);

    template <bool getSP, u16 rdIndex>
    void Thumb_GetRelativeAddress(u16 instruction);

    template <bool sub>
    void Thumb_AddOffsetToStackPointer(u16 instruction);

    template <bool pop, bool bit_pc_lr>
    void Thumb_PushPopRegisters(u16 instruction);

    template <bool load, u16 rbIndex>
    void Thumb_LoadStoreMultiple(u16 instruction);

    template <u16 condition>
    void Thumb_ConditionalBranch(u16 instruction);

    void Thumb_SoftwareInterrupt(u16 instruction);

    void Thumb_UnconditionalBranch(u16 instruction);

    template <bool secondInstruction>
    void Thumb_LongBranchWithLink(u16 instruction);

    // Arithmetic operations:
    u32 ADD(u32 op1, u32 op2, bool set_flags);
    u32 SUB(u32 op1, u32 op2, bool set_flags);
    u32 ADC(u32 op1, u32 op2, bool set_flags);
    u32 SBC(u32 op1, u32 op2, bool set_flags);

    void UpdateNZFlags(u32 result);
    void UpdateCFlag(u32 op1, u32 op2, bool isSub, u32 carry = 0);
    void UpdateVFlag(u32 op1, u32 op2, u32 result, bool isSub);


protected:
    CPU_Registers cpuState;
    std::array<uint32_t, 16> visibleRegisters{};    // R0 - R14 contain data, R15 contains address of next instruction (PC)
    uint32_t cpsr = 0;                              // Current Program Status Register

    std::array<uint32_t, 5> fiqR8_R12{};
    std::array<uint32_t, 5> sharedR8_R12{}; // User registers, shared with some other modes
    std::array<uint32_t, 6> bankedR13s{};   // Stack pointers for FIQ, IRQ, Supervisor, Abort, Undefined
    std::array<uint32_t, 6> bankedR14s{};   // Link registers for FIQ, IRQ, Supervisor, Abort, Undefined
    std::array<uint32_t, 5> spsr{};         // SPSR for each banked mode

    Pipeline pipeline;
    
    bool halted = false;

    void Fetch();
    void Decode();
    void Execute();

    bool ConditionPassed(Condition condition);
    void Decode_ARM(u32 instruction, InstructionHandler& handler);
    void Decode_Thumb(u32 instruction, InstructionHandler& handler);

    void AdvanceInstructionPipeline(); 
    void FlushPipeline();

    void HandleHalt();

private:
    EmulatorCore* core;
    GBA_Bus& bus;

    uint32_t totalCycles;
    uint32_t currentInstructionCycles;
    uint32_t currentFrameCycles;
    bool nextInstructionFetchIsSequential = false;
    bool nextDataAccessIsSequential = false;

    inline void WriteRegister(int index, uint32_t value) { cpuState.registers[index] = value; }

    // Inline allows us to initialize here
    inline static const std::array<bool, 256> conditionTable = TableGenerator::GenerateTable_Condition(); // Condition lookup table, precomputed
    inline static const std::array<Handler_ARM, 4096> armInstructionTable = TableGenerator::GenerateTable_ARM(); // ARM instruction lookup table, precomputed
    inline static const std::array<Handler_Thumb, 1024> thumbInstructionTable = TableGenerator::GenerateTable_Thumb(); // ARM instruction lookup table, precomputed
    
};

#include "CPU/Instructions/Thumb/Instructions.inl"
#include "CPU/Instructions/Arithmetic.inl"

//https://problemkaputt.de/gbatek-arm-cpu-reference.htm - ARM CPU Reference
//https://developer.arm.com/documentation/ddi0210/c/Introduction/Instruction-set-summary/ARM-instruction-summary?lang=en

/* TODO NEXT:

- Ensure any sort of read/writes are properly called/handled. (at call sites)
- Fix cpu.read/write function signatures
- Ensure cycles are added correctly and handled
- Start working on IO and PPU
- Thumb instruction set (surely fast)


TESTING REPOSITORIES:

Arm-Wrestler:
https://github.com/shonumi/Emu-Docs/tree/master/GameBoy%20Advance/test_roms/arm_wrestler

Specific tests:
- jsmolka
https://github.com/jsmolka/gba-tests/tree/3fc2dc019f91180585c7f71d1d68c271baa331fe/arm

-png183
https://github.com/png183/gba-tests/tree/master - has ROM's

https://github.com/mgba-emu/suite - timings
*/