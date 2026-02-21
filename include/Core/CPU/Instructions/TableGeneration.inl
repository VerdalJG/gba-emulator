#pragma once
#include "Core/GBA_CPU.hpp"
#include "Core/CPU/ARM/Opcodes.hpp"
#include "Core/CPU/Thumb/Opcodes.hpp"
#include "Core/CPU/Instructions/ARM/Conditions.hpp"

inline constexpr std::array<ARM_Handler, ARM_Opcode_Count> GBA_CPU::GenerateARMInstructionTable()
{
    std::array<ARM_Handler, ARM_Opcode_Count> table;

    table[ARM_Opcode::ARM_DataProcessing]        = &GBA_CPU::ARM_DataProcessing;
    table[ARM_Opcode::ARM_PSRTransfer]           = &GBA_CPU::ARM_PSRTransfer;
    table[ARM_Opcode::ARM_Multiply]              = &GBA_CPU::ARM_Multiply;
    table[ARM_Opcode::ARM_MultiplyLong]          = &GBA_CPU::ARM_MultiplyLong;
    table[ARM_Opcode::ARM_SingleDataTransfer]    = &GBA_CPU::ARM_SingleDataTransfer;
    table[ARM_Opcode::ARM_HalfwordDataTransfer]  = &GBA_CPU::ARM_HalfwordDataTransfer;
    table[ARM_Opcode::ARM_BlockDataTransfer]     = &GBA_CPU::ARM_BlockDataTransfer;
    table[ARM_Opcode::ARM_SingleDataSwap]        = &GBA_CPU::ARM_SingleDataSwap;
    table[ARM_Opcode::ARM_Branch]                = &GBA_CPU::ARM_Branch;
    table[ARM_Opcode::ARM_BranchAndExchange]     = &GBA_CPU::ARM_BranchAndExchange;
    table[ARM_Opcode::ARM_SoftwareInterrupt]     = &GBA_CPU::ARM_SoftwareInterrupt;
    table[ARM_Opcode::ARM_UndefinedInstruction]  = &GBA_CPU::ARM_UndefinedInstruction;
    table[ARM_Opcode::ARM_Coprocessor]           = &GBA_CPU::ARM_UndefinedInstruction; // maps to same handler
    table[ARM_Opcode::ARM_Invalid]               = &GBA_CPU::ARM_UndefinedInstruction; // fallback
    table[ARM_Opcode::ARM_Suppressed]            = nullptr;                            // no-op 

    return table;
}

inline constexpr std::array<Thumb_Handler, Thumb_Opcode_Count> GBA_CPU::GenerateThumbInstructionTable()
{
    std::array<Thumb_Handler, Thumb_Opcode_Count> table;

    table[Thumb_Opcode::Thumb_MoveShiftedRegister]      = &GBA_CPU::Thumb_MoveShiftedRegister;
    table[Thumb_Opcode::Thumb_AddSubtract]              = &GBA_CPU::Thumb_AddSubtract;
    table[Thumb_Opcode::Thumb_ImmediateOp]              = &GBA_CPU::Thumb_ImmediateOp;
    table[Thumb_Opcode::Thumb_ALU]                      = &GBA_CPU::Thumb_ALU;
    table[Thumb_Opcode::Thumb_HiRegisterOp]             = &GBA_CPU::Thumb_HiRegisterOp;
    table[Thumb_Opcode::Thumb_LoadPCRelative]           = &GBA_CPU::Thumb_LoadPCRelative;
    table[Thumb_Opcode::Thumb_LoadStoreRegisterOffset]  = &GBA_CPU::Thumb_LoadStoreRegisterOffset;
    table[Thumb_Opcode::Thumb_LoadStoreSignExtended]    = &GBA_CPU::Thumb_LoadStoreSignExtended;
    table[Thumb_Opcode::Thumb_LoadStoreImmediateOffset] = &GBA_CPU::Thumb_LoadStoreImmediateOffset;
    table[Thumb_Opcode::Thumb_LoadStoreHalfword]        = &GBA_CPU::Thumb_LoadStoreHalfword;
    table[Thumb_Opcode::Thumb_LoadStoreSPRelative]      = &GBA_CPU::Thumb_LoadStoreSPRelative;
    table[Thumb_Opcode::Thumb_GetRelativeAddress]       = &GBA_CPU::Thumb_GetRelativeAddress;
    table[Thumb_Opcode::Thumb_AddOffsetToStackPointer]  = &GBA_CPU::Thumb_AddOffsetToStackPointer;
    table[Thumb_Opcode::Thumb_PushPopRegisters]         = &GBA_CPU::Thumb_PushPopRegisters;
    table[Thumb_Opcode::Thumb_LoadStoreMultiple]        = &GBA_CPU::Thumb_LoadStoreMultiple;
    table[Thumb_Opcode::Thumb_ConditionalBranch]        = &GBA_CPU::Thumb_ConditionalBranch;
    table[Thumb_Opcode::Thumb_SoftwareInterrupt]        = &GBA_CPU::Thumb_SoftwareInterrupt;
    table[Thumb_Opcode::Thumb_UnconditionalBranch]      = &GBA_CPU::Thumb_UnconditionalBranch;
    table[Thumb_Opcode::Thumb_LongBranchWithLink]       = &GBA_CPU::Thumb_LongBranchWithLink;

    return table;
}
