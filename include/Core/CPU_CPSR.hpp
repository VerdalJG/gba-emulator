#pragma once

struct CPSRFlags
{
    bool N = false;
    bool Z = false;
    bool C = false;
    bool V = false;

    CPSRFlags()
    {
    }
    

    CPSRFlags(bool N, bool Z, bool C, bool V)
    {
        this->N = N;
        this->Z = Z;
        this->C = C;
        this->V = V;
    }
};

// TODO: think about some operations that dont need op2

/*General ARM rules for NZ flags

All data-processing instructions (AND, ADD, SUB, ORR, MOV, CMP, TST, etc.) can update N and Z (and sometimes C/V).

Whether they actually update depends on the S bit in the instruction encoding.

If S=0: no flags updated.

If S=1: flags updated.

Special case: CMP, CMN, TST, TEQ always update flags. */


// CPSR Notes
// CMP, CMN, TST (V unchanged), TEQ (V unchanged)
// Always update flags (act as if S is implicit) but don’t write to a destination register.

// MOVS, MVNS	Update N, Z, and C (from shifter operand if applicable), but not V.
// Logical ops (ANDS, ORRS, etc.)	Update N, Z, and C; V unchanged.
// Arithmetic ops (ADDS, SUBS, etc.)	Update all four (N, Z, C, V).

// 5. Mode restrictions

// In privileged modes, if you write to CPSR fields that control processor mode or 
// interrupts, you need the right access level.

// In user mode, only the condition flags (NZCVQ) are writable.

// 6. Thumb mode

// In Thumb state, not all instructions can update flags, 
// but when they do (e.g., ADDS), they follow the same CPSR flag rules.


// ARITHMETIC VS LOGICAL OPS
// Arithmetic ops (ADD, ADC, SUB, SBC, RSB, RSC, CMP, CMN) Flags (N, Z, C, V) 
// are based on the result of the arithmetic operation.

// Logical ops (AND, EOR, ORR, BIC, MOV, MVN, TST, TEQ)	N and Z come from the logical result, 
// but C comes from the shifter carry-out (from Operand2’s shift), and V is unchanged.