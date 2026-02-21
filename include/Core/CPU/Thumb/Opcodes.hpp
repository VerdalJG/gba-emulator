#pragma once

#include "Core/CPU/InstructionPipeline.hpp"
#include "Utils/Integers.hpp"

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

    Thumb_Opcode_Count
};