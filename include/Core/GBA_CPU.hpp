#pragma once

#include <cstdint>
#include <array>
#include "Core/CPU/CPU_Modes.hpp"
//#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"
#include "Utils/Logger.hpp"
#include "Utils/Integers.hpp"
#include "Core/CPU/Registers.hpp"
#include "Core/GBA_Bus.hpp"

#include "Core/CPU/InstructionPipeline.hpp"
#include "Core/CPU/ARM/Opcodes.hpp"
#include "Core/CPU/Thumb/Opcodes.hpp"

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
    
    // inline uint32_t GetValueAtUserRegister(int registerIndex) { return sharedR8_R12[8 - registerIndex]; }
    // inline void SetValueAtUserRegister(int registerIndex, uint32_t value) { sharedR8_R12[8 - registerIndex] = value; }
    inline void AdvanceProgramCounter() { cpuState.r15 += (IsThumbMode() ? 2u : 4u); }

    // CPSR functions
    inline uint GetCPSR() { return cpuState.cpsr.value; }
    inline uint GetCPSR_N() { return cpuState.cpsr.fields.n; }
    inline uint GetCPSR_Z() { return cpuState.cpsr.fields.z; }
    inline uint GetCPSR_C() { return cpuState.cpsr.fields.c; }
    inline uint GetCPSR_V() { return cpuState.cpsr.fields.v; }
    inline bool IsThumbMode() { return cpuState.cpsr.fields.thumb; }
    
    inline Mode GetCurrentMode() { return static_cast<Mode>(cpuState.cpsr.fields.mode); }
    // inline void UpdateCPSR(uint32_t bits, uint32_t bitsToUpdate = 0xFFFFFFFF) { cpsr = (cpsr & ~bitsToUpdate) | bits; }
    
    // SPSR functions
    inline bool CurrentModeHasSPSR() { return (GetCurrentMode() != Mode::USR) && (GetCurrentMode() != Mode::SYS); } 
    
    
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

    // Memory access
    u32 Read8(u32 address, uint access);
    u32 Read16(u32 address, uint access);
    u32 Read32(u32 address, uint access);

    void Write8(u32 address, u8 value, uint access);
    void Write16(u32 address, u16 value, uint access);
    void Write32(u32 address, u32 value, uint access);

    // Used for LDR and SWP
    u32 Read16_Rotated(u32 address, uint access);
    u32 Read32_Rotated(u32 address, uint access);

    u32 Read8_Signed(u32 address, uint access);
    u32 Read16_Signed(u32 address, uint access);

    void InvalidateSequentiality(); 


private:
    CPU_Registers cpuState;

    Pipeline pipeline;
    
    bool halted = false;

    void Fetch();
    void Decode();
    void Execute();

    bool ConditionPassed(Condition condition);
    
    ARM_Opcode Decode_ARM(u32 instruction);
    ARM_Opcode Decode_ARM_Pattern00(u32 instruction);
    ARM_Opcode Decode_ARM_Pattern01(u32 instruction);
    ARM_Opcode Decode_ARM_Pattern10(u32 instruction);
    ARM_Opcode Decode_ARM_Pattern11(u32 instruction);

    Thumb_Opcode Decode_Thumb(u16 instruction);

    void AdvanceInstructionPipeline(); 
    void FlushPipeline();

    // TODO: Implement
    StatusRegister GetCurrentSPSR() { return StatusRegister{}; };

    StatusRegister* currentSPSR;

    void HandleHalt();

    EmulatorCore* core;
    GBA_Bus& bus;

    uint32_t totalCycles;
    uint32_t currentInstructionCycles;
    uint32_t currentFrameCycles;
    bool nextInstructionFetchIsSequential = false;
    bool nextDataAccessIsSequential = false;

    void WriteRegister(uint index, u32 value); 

    // ARM instructions:
    void ARM_DataProcessing(u32 instruction);
    void ARM_PSRTransfer(u32 instruction);
    void ARM_Multiply(u32 instruction);
    void ARM_MultiplyLong(u32 instruction);
    void ARM_SingleDataTransfer(u32 instruction);
    void ARM_HalfwordDataTransfer(u32 instruction);
    void ARM_BlockDataTransfer(u32 instruction);
    void ARM_SingleDataSwap(u32 instruction);
    void ARM_Branch(u32 instruction);
    void ARM_BranchAndExchange(u32 instruction);
    void ARM_SoftwareInterrupt(u32 instruction);
    void ARM_UndefinedInstruction(u32 instruction);

    // Thumb instructions:
    void Thumb_MoveShiftedRegister(u16 instruction);
    void Thumb_AddSubtract(u16 instruction);
    void Thumb_ImmediateOp(u16 instruction);
    void Thumb_ALU(u16 instruction);
    void Thumb_HiRegisterOp(u16 instruction);
    void Thumb_LoadPCRelative(u16 instruction);
    void Thumb_LoadStoreRegisterOffset(u16 instruction);
    void Thumb_LoadStoreSignExtended(u16 instruction);
    void Thumb_LoadStoreImmediateOffset(u16 instruction);
    void Thumb_LoadStoreHalfword(u16 instruction);
    void Thumb_LoadStoreSPRelative(u16 instruction);
    void Thumb_GetRelativeAddress(u16 instruction);
    void Thumb_AddOffsetToStackPointer(u16 instruction);
    void Thumb_PushPopRegisters(u16 instruction);
    void Thumb_LoadStoreMultiple(u16 instruction);
    void Thumb_ConditionalBranch(u16 instruction);
    void Thumb_SoftwareInterrupt(u16 instruction);
    void Thumb_UnconditionalBranch(u16 instruction);
    void Thumb_LongBranchWithLink(u16 instruction);

    // Arithmetic operations:
    u32 ADD(u32 op1, u32 op2, bool set_flags);
    u32 SUB(u32 op1, u32 op2, bool set_flags);
    u32 ADC(u32 op1, u32 op2, bool set_flags);
    u32 SBC(u32 op1, u32 op2, bool set_flags);

    // CPSR / SPSR operations:
    void RestoreCPSRFromSPSR(ExceptionBank oldExceptionModeIndex);
    void SaveCPSRIntoSPSR(ExceptionBank exceptionModeIndex);

    void UpdateNZFlags(u32 result);
    void UpdateCFlag(u32 op1, u32 op2, bool isSub, u32 carry = 0);
    void UpdateVFlag(u32 op1, u32 op2, u32 result, bool isSub);

    static constexpr std::array<ARM_Handler, ARM_Opcode_Count> GenerateARMInstructionTable();
    static constexpr std::array<Thumb_Handler, Thumb_Opcode_Count> GenerateThumbInstructionTable();

    // Inline allows us to initialize here
    inline static const std::array<bool, 256> conditionTable = GenerateConditionTable(); // Condition lookup table, precomputed
    inline static const std::array<ARM_Handler, ARM_Opcode_Count> armDispatchTable = GenerateARMInstructionTable(); // ARM instruction lookup table, precomputed
    inline static const std::array<Thumb_Handler, Thumb_Opcode_Count> thumbDispatchTable = GenerateThumbInstructionTable(); // Thumb instruction lookup table, precomputed
};

#include "CPU/Instructions/TableGeneration.inl"
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