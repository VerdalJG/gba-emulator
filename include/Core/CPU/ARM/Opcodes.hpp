#pragma once
#include "Core/CPU/InstructionPipeline.hpp"

#include "Utils/Integers.hpp"

enum ARM_Opcode : u8
{
    // --- Data Processing Class ---
    ARM_DataProcessing,          // AND/EOR/SUB/.../MOV/etc (all operand2 forms)

    // --- PSR Transfers ---
    ARM_PSRTransfer,             // MRS + MSR (immediate & register)

    // --- Multiply ---
    ARM_Multiply,                // MUL / MLA
    ARM_MultiplyLong,            // UMULL / UMLAL / SMULL / SMLAL

    // --- Memory ---
    ARM_SingleDataTransfer,      // LDR / STR
    ARM_HalfwordDataTransfer,    // LDRH/LDRSH/LDRSB/STRH (reg+imm variants unified)
    ARM_BlockDataTransfer,       // LDM / STM
    ARM_SingleDataSwap,          // SWP / SWPB

    // --- Branch ---
    ARM_Branch,                  // B / BL
    ARM_BranchAndExchange,       // BX

    // --- Exceptions ---
    ARM_SoftwareInterrupt,       // SWI
    ARM_UndefinedInstruction,

    // --- Coprocessor (undefined on GBA ARM7TDMI) ---
    ARM_Coprocessor,

    ARM_Invalid,
    ARM_Suppressed,
    ARM_Opcode_Count
};

enum class ARM_ALUOp
{
    AND = 0, 
    EOR = 1, 
    SUB = 2, 
    RSB = 3,
    ADD = 4, 
    ADC = 5, 
    SBC = 6, 
    RSC = 7,
    TST = 8, 
    TEQ = 9, 
    CMP = 10, 
    CMN = 11,
    ORR = 12, 
    MOV = 13, 
    BIC = 14, 
    MVN = 15
};
