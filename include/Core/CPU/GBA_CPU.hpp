#pragma once

#include <cstdint>
#include <array>
#include "Core/CPU/CPU_Modes.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"
#include "Utils/Logger.hpp"

class GBA_Memory;
class EmulatorCore;

// Emulates the ARM7TDMI, ARMv4t core
class GBA_CPU 
{
public:
    GBA_CPU(EmulatorCore* core);
    ~GBA_CPU();

    void Reset();            // Reset to CPU initial state
    void Step();             // Fetch, decode, and execute loop
    void RequestInterrupt(); // Triggered by emulator core

    // Register functions
    inline uint32_t GetValueAtRegister(int registerIndex) { return visibleRegisters[registerIndex]; }
    inline void SetValueAtRegister(int registerIndex, uint32_t value) 
    { 
        visibleRegisters[registerIndex] = value; 
        if (registerIndex == PC_INDEX) FlushPipeline();
    }

    inline uint32_t GetValueAtUserRegister(int registerIndex) { return sharedR8_R12[8 - registerIndex]; }
    inline void SetValueAtUserRegister(int registerIndex, uint32_t value) { sharedR8_R12[8 - registerIndex] = value; }

    // CPSR functions
    inline uint32_t GetCPSR() { return cpsr; }
    inline bool GetCPSR_N() { return (cpsr >> 31) & 1; }
    inline bool GetCPSR_Z() { return (cpsr >> 30) & 1; }
    inline bool GetCPSR_C() { return (cpsr >> 29) & 1; }
    inline bool GetCPSR_V() { return (cpsr >> 28) & 1; }
    inline bool IsThumbMode() { return (cpsr >> 5) & 1; }
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

    GBA_Memory& GetMemorySystem();
    EmulatorCore* GetEmulatorCore();
    void Log(const std::string& message, LogType logType, const char* functionName = nullptr);

    static const int SP_INDEX = 13;
    static const int LR_INDEX = 14;
    static const int PC_INDEX = 15;

protected:
    std::array<uint32_t, 16> visibleRegisters{};    // R0 - R14 contain data, R15 contains address of next instruction (PC)
    uint32_t cpsr = 0;                              // Current Program Status Register

    std::array<uint32_t, 5> fiqR8_R12{};
    std::array<uint32_t, 5> sharedR8_R12{}; // User registers, shared with some other modes
    std::array<uint32_t, 6> bankedR13s{};   // Stack pointers for FIQ, IRQ, Supervisor, Abort, Undefined
    std::array<uint32_t, 6> bankedR14s{};   // Link registers for FIQ, IRQ, Supervisor, Abort, Undefined
    std::array<uint32_t, 5> spsr{};         // SPSR for each banked mode

    std::array<Instruction, 3> instructionPipeline{}; // [0] = fetch, [1] = decode, [2] = execute

    void Fetch();
    void Decode();
    void Execute();

    void AdvanceInstructionPipeline();
    void AdvanceProgramCounter(); 
    void FlushPipeline();
    InstructionFunction DecodeInstruction(uint32_t instruction);
    void HandleUndefinedBehavior(uint32_t instruction);

private:
    EmulatorCore* core;
};



//https://problemkaputt.de/gbatek-arm-cpu-reference.htm - ARM CPU Reference
//https://developer.arm.com/documentation/ddi0210/c/Introduction/Instruction-set-summary/ARM-instruction-summary?lang=en

/* TODO NEXT:

- Refactor Halfword data transfer - done
- Finish Single data transfer and Addressing mode 2 - done
- Revise these vs GBATEK - done
- Move onto decoding the last 2 patterns - done

- Setup proper Step() function - done
- Setup real pipeline for instructions - done
- Move decode instruction to CPU_Decoder
- Potentially move specific instruction structs to their specific hpp's to not clutter InstructionHelpers.hpp/cpp
- Thumb instruction set (surely fast)

*/