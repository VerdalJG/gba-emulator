#pragma once
#include "Core/CPU/InstructionPipeline.hpp"

#include "Utils/Integers.hpp"

#include <string>

enum Thumb_Opcode : u8
{
    // --- Register Operations ---
    Thumb_MoveShiftedRegister,          // LSL / LSR / ASR (immediate)
    Thumb_AddSubtract,                  // ADD / SUB (register & immediate forms)
    Thumb_ImmediateOp,                  // MOV / CMP / ADD / SUB (immediate)
    Thumb_ALU,                          // AND/EOR/ADC/SBC/ROR/TST/NEG/CMP/CMN/ORR/MUL/BIC/MVN
    Thumb_HiRegisterOp,                 // ADD / CMP / MOV (high registers)

    // --- Memory ---
    Thumb_LoadPCRelative,               // LDR (PC-relative literal load)
    Thumb_LoadStoreRegisterOffset,      // LDR / STR (register offset)
    Thumb_LoadStoreSignExtended,        // LDRSB / LDRSH / STRH (register offset)
    Thumb_LoadStoreImmediateOffset,     // LDR / STR (immediate offset)
    Thumb_LoadStoreHalfword,            // LDRH / STRH (immediate offset)
    Thumb_LoadStoreSPRelative,          // LDR / STR (SP-relative)

    // --- Address Calculation ---
    Thumb_GetRelativeAddress,           // ADD Rd, PC/SP, #imm
    Thumb_AddOffsetToStackPointer,      // ADD / SUB SP, #imm

    // --- Memory Multiple Transfer ---
    Thumb_PushPopRegisters,             // PUSH / POP
    Thumb_LoadStoreMultiple,            // LDMIA / STMIA

    // --- Jumps and Calls ---
    Thumb_ConditionalBranch,            // B<cond>
    Thumb_UnconditionalBranch,          // B
    Thumb_LongBranchWithLink,           // BL (two-instruction sequence)
    Thumb_SoftwareInterrupt,            // SWI

    Thumb_Invalid,
    Thumb_Opcode_Count
};

enum class Thumb_ALUOp {
    AND = 0,
    EOR = 1,
    LSL = 2,
    LSR = 3,
    ASR = 4,
    ADC = 5,
    SBC = 6,
    ROR = 7,
    TST = 8,
    NEG = 9,
    CMP = 10,
    CMN = 11,
    ORR = 12,
    MUL = 13,
    BIC = 14,
    MVN = 15
};

inline std::string ThumbOpToString(u8 opcode)
{
    switch (static_cast<Thumb_Opcode>(opcode))
    {
        // --- Register Operations ---
        case Thumb_MoveShiftedRegister:     return "Thumb_MoveShiftedRegister";
        case Thumb_AddSubtract:             return "Thumb_AddSubtract";
        case Thumb_ImmediateOp:             return "Thumb_ImmediateOp";
        case Thumb_ALU:                     return "Thumb_ALU";
        case Thumb_HiRegisterOp:            return "Thumb_HiRegisterOp";

        // --- Memory ---
        case Thumb_LoadPCRelative:          return "Thumb_LoadPCRelative";
        case Thumb_LoadStoreRegisterOffset: return "Thumb_LoadStoreRegisterOffset";
        case Thumb_LoadStoreSignExtended:   return "Thumb_LoadStoreSignExtended";
        case Thumb_LoadStoreImmediateOffset:return "Thumb_LoadStoreImmediateOffset";
        case Thumb_LoadStoreHalfword:       return "Thumb_LoadStoreHalfword";
        case Thumb_LoadStoreSPRelative:     return "Thumb_LoadStoreSPRelative";

        // --- Address Calculation ---
        case Thumb_GetRelativeAddress:      return "Thumb_GetRelativeAddress";
        case Thumb_AddOffsetToStackPointer: return "Thumb_AddOffsetToStackPointer";

        // --- Memory Multiple Transfer ---
        case Thumb_PushPopRegisters:        return "Thumb_PushPopRegisters";
        case Thumb_LoadStoreMultiple:       return "Thumb_LoadStoreMultiple";

        // --- Jumps and Calls ---
        case Thumb_ConditionalBranch:       return "Thumb_ConditionalBranch";
        case Thumb_UnconditionalBranch:     return "Thumb_UnconditionalBranch";
        case Thumb_LongBranchWithLink:      return "Thumb_LongBranchWithLink";
        case Thumb_SoftwareInterrupt:       return "Thumb_SoftwareInterrupt";

        case Thumb_Invalid:                 return "Thumb_Invalid";
        case Thumb_Opcode_Count:            return "Thumb_Opcode_Count";

        default:                           return "Unknown Thumb Opcode";
    }
}