#pragma once

#include <cstdint>
#include <array>
#include "Core/CPU/CPU_Modes.hpp"
#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"
#include "Utils/Logger.hpp"
#include "Utils/Integer.hpp"
#include "Core/CPU/Registers.hpp"

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

    CPU_Registers cpuState;

    // Register functions
    inline uint32_t GetValueAtRegister(int registerIndex) { return visibleRegisters[registerIndex]; }
    inline void SetValueAtRegister(int registerIndex, uint32_t value) 
    { 
        if (registerIndex == PC_INDEX)
        {
            FlushPipeline();
            value &= ~3;
        } 
        visibleRegisters[registerIndex] = value;  
    }

    inline uint32_t GetValueAtUserRegister(int registerIndex) { return sharedR8_R12[8 - registerIndex]; }
    inline void SetValueAtUserRegister(int registerIndex, uint32_t value) { sharedR8_R12[8 - registerIndex] = value; }
    inline void AdvanceProgramCounter() { cpuState.r15 += (IsThumbMode() ? 2u : 4u); }

    // CPSR functions
    inline uint32_t GetCPSR() { return cpsr; }
    inline uint GetCPSR_N() { return cpuState.cpsr.fields.n; }
    inline uint GetCPSR_Z() { return cpuState.cpsr.fields.z; }
    inline uint GetCPSR_C() { return cpuState.cpsr.fields.c; }
    inline uint GetCPSR_V() { return cpuState.cpsr.fields.v; }
    inline bool IsThumbMode() { return cpuState.cpsr.fields.thumb; }
    inline OperatingMode GetCurrentOperatingMode() { return static_cast<OperatingMode>(cpsr & 0x1F); }
    inline void UpdateCPSR(uint32_t bits, uint32_t bitsToUpdate = 0xFFFFFFFF) { cpsr = (cpsr & ~bitsToUpdate) | bits; }
    
    // SPSR functions
    inline bool CurrentModeHasSPSR() 
    { 
        return GetCurrentOperatingMode() != OperatingMode::User && GetCurrentOperatingMode() != OperatingMode::System; 
    }
    
    void RestoreCPSRFromSPSR(int oldExceptionModeIndex);
    void SaveCPSRIntoSPSR(int exceptionModeIndex);
    void UpdateVisibleRegistersForMode(OperatingMode newMode);

    bool GetHalted() { return halted; }
    void SetHalted(bool shouldHalt) { halted = shouldHalt; }

    void AddCycles(uint32_t cycles);
    uint32_t GetTotalCycles() { return totalCycles; }
    uint32_t GetCurrentInstructionCycles() { return currentInstructionCycles; }
    uint32_t GetCurrentFrameCycles() { return currentFrameCycles; }

    EmulatorCore* GetEmulatorCore();
    void Log(const std::string& message, LogType logType, const char* functionName = nullptr);

    uint8_t Read8_Bus(uint32_t address);
    uint16_t Read16_Bus(uint32_t address);
    uint32_t Read32_Bus(uint32_t address);

    void Write8_Bus(uint32_t address, uint8_t value);
    void Write16_Bus(uint32_t address, uint16_t value);
    void Write32_Bus(uint32_t address, uint32_t value);

    void InvalidateSequentiality(); 

    static const int SP_INDEX = 13;
    static const int LR_INDEX = 14;
    static const int PC_INDEX = 15;

    using Handler_ARM = void (GBA_CPU::*)(u32); 
    using Handler_Thumb = void (GBA_CPU::*)(u16);

    //#include "CPU/Instructions/ARM/Handler.hpp"

    // Thumb instructions:
    template <u16 shiftOp, u16 immediate_5>
    void Thumb_MoveShiftedRegister(u16 instruction);

    template <bool subtract, bool immediate, u16 operand>
    void Thumb_AddSubtract(u16 instruction);

    template <u16 opcode, u16 rdIndex>
    void Thumb_ImmediateOp(u16 instruction);

    template <u16 opcode>
    void Thumb_ALU(u16 instruction);

    // Arithmetic operations:
    u32 ADD(u32 op1, u32 op2, bool set_flags);
    u32 SUB(u32 op1, u32 op2, bool set_flags);
    u32 ADC(u32 op1, u32 op2, bool set_flags);
    u32 SBC(u32 op1, u32 op2, bool set_flags);
    void UpdateNZFlags(u32 result);
    void UpdateCFlag(u32 op1, u32 op2, bool isSub, u32 carry = 0);
    void UpdateVFlag(u32 op1, u32 op2, u32 result, bool isSub);


protected:
    std::array<uint32_t, 16> visibleRegisters{};    // R0 - R14 contain data, R15 contains address of next instruction (PC)
    uint32_t cpsr = 0;                              // Current Program Status Register

    std::array<uint32_t, 5> fiqR8_R12{};
    std::array<uint32_t, 5> sharedR8_R12{}; // User registers, shared with some other modes
    std::array<uint32_t, 6> bankedR13s{};   // Stack pointers for FIQ, IRQ, Supervisor, Abort, Undefined
    std::array<uint32_t, 6> bankedR14s{};   // Link registers for FIQ, IRQ, Supervisor, Abort, Undefined
    std::array<uint32_t, 5> spsr{};         // SPSR for each banked mode

    std::array<Instruction, 3> instructionPipeline{}; // [0] = fetch, [1] = decode, [2] = execute
    bool halted = false;

    void Fetch();
    void Decode();
    void Execute();

    void AdvanceInstructionPipeline(); 
    void FlushPipeline();

    void HandleUndefinedBehavior(uint32_t instruction);

    void HandleHalt();

private:
    EmulatorCore* core;
    GBA_Bus& bus;

    uint32_t totalCycles;
    uint32_t currentInstructionCycles;
    uint32_t currentFrameCycles;
    bool nextInstructionFetchIsSequential = false;
    bool nextDataAccessIsSequential = false;

    static std::array<bool, 256> conditionTable; // Condition lookup table, precomputed
    static std::array<Handler_ARM, 4096> armInstructionTable; // ARM instruction lookup table, precomputed
    static std::array<Handler_Thumb, 4096> thumbInstructionTable; // ARM instruction lookup table, precomputed
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