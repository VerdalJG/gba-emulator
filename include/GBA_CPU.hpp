#pragma once

#include <cstdint>
#include <array>
#include "GBA_Memory.hpp"
#include "InstructionHelpers.hpp"

enum class CPUMode
{
    ARM,
    Thumb
};

// Emulates the ARM7TDMI
class GBA_CPU 
{
public:
    GBA_CPU(GBA_Memory& memory);
    ~GBA_CPU();
    using InstructionFunction = void (GBA_CPU::*)(uint32_t); // Instruction Function Pointer alias

    void Reset();            // Reset to CPU initial state
    void Step();             // Fetch, decode, and execute loop
    void RequestInterrupt(); // Triggered by emulator core

    uint32_t GetValueAtRegister(int registerIndex);
    inline uint32_t GetCPSR() { return cpsr; }
    inline uint32_t GetSPSR() { return spsr; }
    inline CPUMode GetCPUMode() { return mode; }

protected:
    std::array<uint32_t, 16> registers{}; // R0 - R14 contain data, R15 contains address of next instruction (PC)
    uint32_t cpsr = 0;                    // Current Program Status Register
    uint32_t spsr = 0;                    // Saved Program Status Register
    CPUMode mode;                      // Thumb mode flag


    uint32_t ReadProgramCounter(bool isPartOfInstruction); // Result is different if we are reading PC as a fetch or part of an instruction
    uint8_t GetConditionBits(uint32_t instruction); // Only ARM mode uses condition bits
    InstructionFunction DecodeARMInstruction(uint32_t instruction);
    void ApplyCPSRFlags(CPSRFlags flags);
    

    static constexpr int DATA_PROCESSING_OPCODE_COUNT = 16;
    static InstructionFunction dataProcessingTable[DATA_PROCESSING_OPCODE_COUNT];


    // In order of opcodes 0-15
    void LogicalAND(uint32_t instruction);
    void LogicalExclusiveOR(uint32_t instruction);
    void Subtract(uint32_t instruction);
    void ReverseSubtract(uint32_t instruction);
    void Add(uint32_t instruction);
    void AddWithCarry(uint32_t instruction);
    void SubtractWithCarry(uint32_t instruction);
    void ReverseSubtractWithCarry(uint32_t instruction);
    void Test(uint32_t instruction);
    void TestEquivalence(uint32_t instruction);
    void Compare(uint32_t instruction);
    void CompareNegated(uint32_t instruction);
    void LogicalOR(uint32_t instruction);
    void Move(uint32_t instruction);
    void BitClear(uint32_t instruction);
    void MoveNot(uint32_t instruction);
    void HandleUndefinedBehavior(uint32_t instruction);

    
    InstructionFunction DecodePattern00(uint32_t instruction);
    InstructionFunction DecodePattern01(uint32_t instruction);
    InstructionFunction DecodePattern10(uint32_t instruction);
    InstructionFunction DecodePattern11(uint32_t instruction);

private:
    GBA_Memory& memorySystem;
};

//https://problemkaputt.de/gbatek-arm-cpu-reference.htm - ARM CPU Reference
//https://developer.arm.com/documentation/ddi0210/c/Introduction/Instruction-set-summary/ARM-instruction-summary?lang=en
