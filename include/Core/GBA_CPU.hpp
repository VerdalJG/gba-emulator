#pragma once

#include <cstdint>
#include <array>
#include "Core/GBA_Memory.hpp"
#include "Core/InstructionHelpers.hpp"


enum class OperatingMode
{
    User,
    System,
    Supervisor,
    Abort,
    Undefined,
    Interrupt,
    FastInterrupt
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
    inline bool GetCPSR_N() { return (cpsr >> 31) & 1; }
    inline bool GetCPSR_Z() { return (cpsr >> 30) & 1; }
    inline bool GetCPSR_C() { return (cpsr >> 29) & 1; }
    inline bool GetCPSR_V() { return (cpsr >> 28) & 1; }

    inline uint32_t GetSPSR() { return spsr; }
    inline bool IsThumbMode() { return (cpsr >> 5) & 1; }

protected:
    std::array<uint32_t, 16> registers{}; // R0 - R14 contain data, R15 contains address of next instruction (PC)
    uint32_t cpsr = 0;                    // Current Program Status Register
    uint32_t spsr = 0;                    // Saved Program Status Register
    OperatingMode opMode;                 // Operating mode flag

    uint32_t ReadProgramCounter(bool isPartOfInstruction); // Result is different if we are reading PC as a fetch or part of an instruction

    InstructionFunction DecodeARMInstruction(uint32_t instruction);


    bool InstructionConditionCheck(uint8_t conditionByte);
    inline uint8_t GetConditionBits(uint32_t instruction) // Only ARM mode uses condition bits
    {
        return (instruction >> 28) & 0xF;
    }

    inline bool CurrentModeHasSPSR()
    {
        return opMode != OperatingMode::User && opMode != OperatingMode::System;
    }

    // ============================================ CPSR ============================================

    // Wrapper function for handling carry/overflow flags (ADD/ADC)
    void UpdateCPSR_Add(uint32_t result, uint32_t op1, uint32_t op2, bool carryIn = false);
    // Wrapper function for handling carry/overflow flags for subtraction (SUB/SBC)
    void UpdateCPSR_Sub(uint32_t result, uint32_t op1, uint32_t op2, bool carryIn = false);

    void UpdateCPSR_Arithmetic(uint32_t result, uint32_t op1, uint32_t op2, bool isSub, bool carryIn = false);
    void UpdateCPSR_Logical(uint32_t result, bool shifterCarryOut);

    void UpdateCPSR(uint8_t flags, uint8_t flagsToUpdate);

    // CPSR update behaves differently when the destination Register is R15
    void HandleProgramCounterCpsrCase();

    uint32_t CarryFrom(uint64_t result);
    
    // ==============================================================================================

    // ===================================== DATA PROCESSING ========================================

    static constexpr int DATA_PROCESSING_OPCODE_COUNT = 16;
    static InstructionFunction dataProcessingFuncTable[DATA_PROCESSING_OPCODE_COUNT];

    // Templating is better performance wise and 
    // avoids some errors on specific compilers vs 'auto' as parameter
    template <typename Func>
    void ArithmeticOperation(uint32_t instruction, Func operation, bool isSub, bool useCarry = false)
    {
        DataProcessing_DecodedInstruction values = DataProcessing_Decode(instruction, *this);

        uint32_t& rn = registers[values.rnIndex];
        uint32_t& rd = registers[values.rdIndex];
        
        uint32_t op2Value = values.op2.value;
        uint32_t carryIn = useCarry ? GetCPSR_C() : 0;

        rd = operation(rn, op2Value, carryIn);

        // CPSR
        if (!values.setCpsrFlag) return;

        if (values.rdIndex == 15)
        {
            HandleProgramCounterCpsrCase();
            return;
        }

        if (isSub)
        {
            UpdateCPSR_Sub(rd, rn, op2Value, carryIn ? 0 : 1);
        }
        else
        {
            UpdateCPSR_Add(rd, rn, op2Value, carryIn);
        }
    }

    template <typename Func>
    void ArithmeticComparisonOperation(uint32_t instruction, Func operation, bool isSub)
    {
        DataProcessing_DecodedInstruction values = DataProcessing_Decode(instruction, *this);

        uint32_t& rn = registers[values.rnIndex];
        uint32_t op2Value = values.op2.value;

        uint32_t result = operation(rn, op2Value);

        if (isSub)
        {
            UpdateCPSR_Sub(result, rn, op2Value);
        }
        else
        {
            UpdateCPSR_Add(result, rn, op2Value);
        }
    }

    template <typename Func>
    void LogicalOperation(uint32_t instruction, Func operation)
    {
        DataProcessing_DecodedInstruction values = DataProcessing_Decode(instruction, *this);

        uint32_t& rn = registers[values.rnIndex];
        uint32_t& rd = registers[values.rdIndex];
        uint32_t op2Value = values.op2.value;
        bool op2CarryOut = values.op2.carryOut;
    
        rd = operation(rn, op2Value);

        if (!values.setCpsrFlag) return;

        if (values.rdIndex == 15)
        {
            HandleProgramCounterCpsrCase();
            return;
        }

        UpdateCPSR_Logical(rd, op2CarryOut);
    }

    template <typename Func>
    void LogicalTestOperation(uint32_t instruction, Func operation)
    {
        DataProcessing_DecodedInstruction values = DataProcessing_Decode(instruction, *this);

        uint32_t& rn = registers[values.rnIndex];
        uint32_t op2Value = values.op2.value;
        bool op2CarryOut = values.op2.carryOut;

        uint32_t result = operation(rn, op2Value);

        UpdateCPSR_Logical(result, op2CarryOut);
    }

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
    void CompareNegative(uint32_t instruction);
    void LogicalOR(uint32_t instruction);
    void Move(uint32_t instruction);
    void BitClear(uint32_t instruction);
    void MoveNot(uint32_t instruction);
    void HandleUndefinedBehavior(uint32_t instruction);

    // Misc
    void Multiply(uint32_t instruction);
    void MultiplyLong(uint32_t instruction);

    void SMULL(uint32_t& rdLo, uint32_t& rdHi, int64_t& product);
    void SMLAL(uint32_t& rdLo, uint32_t& rdHi, int64_t& product);
    void UMULL(uint32_t& rdLo, uint32_t& rdHi, uint64_t& product);
    void UMLAL(uint32_t& rdLo, uint32_t& rdHi, uint64_t& product);

    void SingleDataSwap(uint32_t instruction);
    void BranchAndExchange(uint32_t instruction);
    void HalfwordDataTransferRegister(uint32_t instruction);
    void HalfwordDataTransferImmediate(uint32_t instruction);

    // ==============================================================================================
    
    InstructionFunction DecodePattern00(uint32_t instruction);
    InstructionFunction DecodePattern01(uint32_t instruction);
    InstructionFunction DecodePattern10(uint32_t instruction);
    InstructionFunction DecodePattern11(uint32_t instruction);

private:
    GBA_Memory& memorySystem;
};

//https://problemkaputt.de/gbatek-arm-cpu-reference.htm - ARM CPU Reference
//https://developer.arm.com/documentation/ddi0210/c/Introduction/Instruction-set-summary/ARM-instruction-summary?lang=en
