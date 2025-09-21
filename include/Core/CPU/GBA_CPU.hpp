#pragma once

#include <cstdint>
#include <array>
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

class GBA_Memory;

#define PROGRAM_COUNTER 15

enum class OperatingMode
{
    User = 0b10000,
    FIQ = 0b10001, // Fast Interrupt
    IRQ = 0b10010, // Interrupt
    Supervisor = 0b10011,
    Abort = 0b10111,
    Undefined = 0b11011,
    System = 0b11111
};

// Emulates the ARM7TDMI, ARMv4t core
class GBA_CPU 
{
public:
    GBA_CPU(GBA_Memory& memory);
    ~GBA_CPU();

    void Reset();            // Reset to CPU initial state
    void Step();             // Fetch, decode, and execute loop
    void RequestInterrupt(); // Triggered by emulator core

    // Register functions
    inline uint32_t GetValueAtRegister(int registerIndex) { return registers[registerIndex]; }
    inline void SetValueAtRegister(int registerIndex, uint32_t value) { registers[registerIndex] = value; }

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
    
    void RestoreCPSRFromSPSR();
    void SaveCPSRIntoSPSR(OperatingMode opMode);

    inline GBA_Memory& GetMemorySystem() { return memorySystem; }

    static const int PC_INDEX = 15;

protected:
    std::array<uint32_t, 16> registers{}; // R0 - R14 contain data, R15 contains address of next instruction (PC)
    uint32_t cpsr = 0;                    // Current Program Status Register

    // Saved Program Status Registers
    uint32_t spsr_fiq = 0;                    
    uint32_t spsr_irq = 0;                   
    uint32_t spsr_supervisor = 0;                    
    uint32_t spsr_abort = 0;
    uint32_t spsr_undefined = 0;  
        
    uint32_t linkRegister_undefined = 0;

    inline void AdvanceProgramCounter()
    {
        registers[15] += (IsThumbMode()) ? 2u : 4u;
    }   

    InstructionFunction DecodeInstruction(uint32_t instruction);
    void HandleUndefinedBehavior(uint32_t instruction);




    // ==============================================================================================

private:
    GBA_Memory& memorySystem;
};

//https://problemkaputt.de/gbatek-arm-cpu-reference.htm - ARM CPU Reference
//https://developer.arm.com/documentation/ddi0210/c/Introduction/Instruction-set-summary/ARM-instruction-summary?lang=en

/* TODO NEXT:

- Refactor Halfword data transfer
- Finish Single data transfer and Addressing mode 2
- Revise these vs GBATEK
- Move onto decoding the last 2 patterns

- Setup proper Step() function
- Setup real pipeline for instructions
- Move decode instruction to CPU_Decoder
- Thumb instruction set (surely fast)

*/