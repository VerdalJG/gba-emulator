#pragma once

#include "Utils/BitOperations.hpp"
#include "Core/CPU/Registers.hpp"

#include <assert.h>

enum class ThumbALUOp {
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

template <u16 shiftOp, u16 immediate_5>
inline void GBA_CPU::Thumb_MoveShiftedRegister(u16 instruction)
{
    u16 rsIndex = ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = ExtractBits<2, 0, u16>(instruction);

    assert(rsIndex < 8 && rdIndex < 8);

    u32 rs = cpuState.registers[rsIndex];

    ApplyShift(shiftOp, rs, immediate_5, GetCPSR_C(), true);

    // Update CPSR flags
    UpdateNZFlags(rs);
    cpuState.cpsr.fields.c = carry;

    cpuState.registers[rdIndex] = rs;
    // TODO: Affect pipeline?
    AdvanceProgramCounter();
}

template <bool subtract, bool immediate, u16 operand>
inline void GBA_CPU::Thumb_AddSubtract(u16 instruction)
{
    u16 rsIndex = ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = ExtractBits<2, 0, u16>(instruction);

    assert(rsIndex < 8 && rdIndex < 8);

    u32 rs = cpuState.registers[rsIndex];
    u32 rn = immediate ? operand : cpuState.registers[operand];

    if (subtract) // CPSR updated inside SUB and ADD
    {
        cpuState.registers[rdIndex] = SUB(rs, rn, true);
    }
    else
    {
        cpuState.registers[rdIndex] = ADD(rs, rn, true);
    }

    // TODO: Affect pipeline?
    AdvanceProgramCounter();
}

template <u16 opcode, u16 rdIndex>
inline void GBA_CPU::Thumb_ImmediateOp(u16 instruction)
{
    u16 immediate_8 = ExtractBits<7, 0, u16>(instruction);

    switch (opcode)
    {
        case 0b00: // MOV
            cpuState.registers[rdIndex] = immediate_8;
            UpdateNZFlags(0);

        case 0b01: // CMP
            SUB(cpuState.registers[rdIndex], immediate_8, true);
        
        case 0b10; // ADD
            cpuState.registers[rdIndex] = ADD(cpuState.registers[rdIndex], immediate_8, true);

        case 0b11; // SUB
            cpuState.registers[rdIndex] = SUB(cpuState.registers[rdIndex], immediate_8, true);
    }

    // TODO: Affect pipeline?
    AdvanceProgramCounter();
}

template <u16 opcode>
inline void GBA_CPU::Thumb_ALU(u16 instruction)
{
    u16 rsIndex = ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = ExtractBits<2, 0, u16>(instruction);

    u32& rs = cpuState.registers[rsIndex];
    u32& rd = cpuState.registers[rdIndex];

    switch (static_cast<ThumbALUOp>(opcode))
    {
        case ThumbALUOp::AND:
        {
            rd &= rs;
            UpdateNZFlags(rd);
            break;
        }            
        
        case ThumbALUOp::EOR:
        {
            rd ^= rs;
            UpdateNZFlags(rd);
            break;
        }
        
        case ThumbALUOp::LSL:
        {
            uint carry = GetCPSR_C();
            const u32 shiftAmount = rs & 0xFF;
            LogicalShiftLeft2(rd, shiftAmount, carry);
            // TODO: add one cycle + pipeline affected
            UpdateNZFlags(rd);
            cpuState.cpsr.fields.c = carry;
            break;
        }

        case ThumbALUOp::LSR:
        {
            uint carry = GetCPSR_C();
            const u32 shiftAmount = rs & 0xFF;
            LogicalShiftRight2(rd, shiftAmount, carry, false);
            // TODO: add one cycle + pipeline affected
            UpdateNZFlags(rd);
            cpuState.cpsr.fields.c = carry;
            break;
        }
            
        case ThumbALUOp::ASR:
        {
            uint carry = GetCPSR_C();
            const u32 shiftAmount = rs & 0xFF;
            ArithmeticShiftRight2(rd, shiftAmount, carry, false);
            // TODO: add one cycle + pipeline affected
            UpdateNZFlags(rd);
            cpuState.cpsr.fields.c = carry;
            break;
        }
            
        case ThumbALUOp::ADC:
        {
            rd = ADC(rd, rs, true);
            break;
        }
            
        
        case ThumbALUOp::SBC:
        {
            rd = SBC(rd, rs, true);
            break;
        }
            
        case ThumbALUOp::ROR:
        {
            uint carry = GetCPSR_C();
            const u32 shiftAmount = rs & 0xFF;
            RotateRight2(rd, shiftAmount, carry, false);
            // TODO: add one cycle + pipeline affected
            UpdateNZFlags(rd);
            cpuState.cpsr.fields.c = carry;
            break;
        }
            
        case ThumbALUOp::TST:
        {
            UpdateNZFlags(rd & rs);
            break;
        }
            
        case ThumbALUOp::NEG:
        {
            rd = SUB(0, rs, true);
            break;
        }
            
        case ThumbALUOp::CMP:
        {
            SUB(rd, rs, true);
            break;
        }
            
        case ThumbALUOp::CMN:
        {
            ADD(rd, rs, true);
            break;
        }
            
        case ThumbALUOp::ORR:
        {
            rd |= rs;
            UpdateNZFlags(rd);
            break;
        }
            
        case ThumbALUOp::MUL: 
        {
            // TODO: Implement and add mI cycles
            break;
        }
            
        case ThumbALUOp::BIC:
        {
            rd &= ~rs;
            UpdateNZFlags(rd);
            break;
        }
            
        case ThumbALUOp::MVN:
        {
            rd = ~rs;
            UpdateNZFlags(rd);
            break;
        }
    }

    AdvanceProgramCounter();
}