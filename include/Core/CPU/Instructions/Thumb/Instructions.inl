#pragma once

#include "Utils/BitOperations.hpp"
#include "Core/CPU/Registers.hpp"
#include "Core/CPU/CPU_Timings.hpp"

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

    u32 rs = cpuState.registers[rsIndex];
    u32 rd = cpuState.registers[rdIndex];

    switch (static_cast<ThumbALUOp>(opcode))
    {
        case ThumbALUOp::AND:
        {
            cpuState.registers[rdIndex] = rd & rs;
            UpdateNZFlags(rd);
            break;
        }            
        
        case ThumbALUOp::EOR:
        {
            cpuState.registers[rdIndex] = rd ^ rs;
            UpdateNZFlags(rd);
            break;
        }
        
        case ThumbALUOp::LSL:
        {
            uint carry = GetCPSR_C();
            const u32 shiftAmount = rs & 0xFF;
            LSL(rd, shiftAmount, carry);
            cpuState.registers[rdIndex] = rd;

            // TODO: add one cycle + pipeline affected
            UpdateNZFlags(rd);
            cpuState.cpsr.fields.c = carry;
            break;
        }

        case ThumbALUOp::LSR:
        {
            uint carry = GetCPSR_C();
            const u32 shiftAmount = rs & 0xFF;
            LSR(rd, shiftAmount, carry, false);
            cpuState.registers[rdIndex] = rd;

            // TODO: add one cycle + pipeline affected
            UpdateNZFlags(rd);
            cpuState.cpsr.fields.c = carry;
            break;
        }
            
        case ThumbALUOp::ASR:
        {
            uint carry = GetCPSR_C();
            const u32 shiftAmount = rs & 0xFF;
            ASR(rd, shiftAmount, carry, false);
            cpuState.registers[rdIndex] = rd;

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
            ROR(rd, shiftAmount, carry, false);
            cpuState.registers[rdIndex] = rd;
            
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
            cpuState.registers[rdIndex] = SUB(0, rs, true);
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
            cpuState.registers[rdIndex] = rd | rs;
            UpdateNZFlags(rd);
            break;
        }
            
        case ThumbALUOp::MUL: 
        {
            u32 cycles = CalculateMultiplierCycles(rs);
            cpuState.registers[rdIndex] = rd * rs;
            UpdateNZFlags(rd);
            // TODO: Add mI cycles properly
            break;
        }
            
        case ThumbALUOp::BIC:
        {
            cpuState.registers[rdIndex] = rd & ~rs;
            UpdateNZFlags(rd);
            break;
        }
            
        case ThumbALUOp::MVN:
        {
            cpuState.registers[rdIndex] = ~rs;
            UpdateNZFlags(rd);
            break;
        }
    }

    AdvanceProgramCounter();
}

template <u16 opcode, u16 msbRd, u16 msbRs>
inline void GBA_CPU::Thumb_HiRegisterOp(u16 instruction)
{
    // Restriction: For ADD/CMP/MOV, MSBs and/or MSBd must be set
    if (opcode < 3 && msbRs == 0 && msbRd == 0)
    {
        // TODO: Affect Pipeline?
        AdvanceProgramCounter();
        return; 
    }

    // MSB allows for access to hi registers
    u16 rsIndex = (msbRs << 4) | ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = (msbRd << 4) | ExtractBits<2, 0, u16>(instruction);

    u32 rs = cpuState.registers[rsIndex];
    u32 rd = cpuState.registers[rdIndex];
    
    switch (opcode)
    {
        case 0: // ADD
        {
            cpuState.registers[rdIndex] = rd + rs;
            if (rdIndex == PC_INDEX)
            {
                cpuState.r15 &= ~1; // Halfword align
                // TODO: Pipeline flush
            }
            else
            {
                // TODO: Affect Pipeline?
                AdvanceProgramCounter();
            }
            break;
        }

        case 1: // CMP
        {
            SUB(rd, rs, true);
            // TODO: Affect Pipeline?
            AdvanceProgramCounter();
            break;
        }
        
        case 2: //MOV
        {
            cpuState.registers[rdIndex] = rs;
            if (rdIndex == PC_INDEX)
            {
                cpuState.r15 &= ~1; // Halfword align
                // TODO: Pipeline flush
            }
            else
            {
                // TODO: Affect Pipeline?
                AdvanceProgramCounter();
            }
            break;
        }

        case 3: // BX
        {
            // Switching to ARM mode or not? (0 = ARM, 1 = THUMB)
            if (rs & 1) // Stay in THUMB
            {
                cpuState.r15 = rs & ~1;
                // TODO: Pipeline flush
            }
            else // Switch to ARM
            {
                cpuState.cpsr.fields.thumb = 0;
                cpuState.r15 = rs & ~2; // Bit 1 is already cleared, this makes it word-aligned
                // TODO: Pipeline flush
                // TODO: Affect Pipeline?
            }
            break;
        }
    }
}

template <u16 rdIndex>
inline void GBA_CPU::Thumb_LoadPCRelative(u16 instruction)
{   
    u16 offset_8 = ExtractBits<7, 0, u16>(instruction); 
    u32 address = (cpuState.r15 & ~ 2) + (offset_8 << 2); // Offset is in steps of 4 (0-1020)

    // Load the value
    u32 rd = cpuState.registers[rdIndex];
    cpuState.registers[rdIndex] = Read32(address);

    // TODO: Affect pipeline
    AdvanceProgramCounter();
    // TODO: Add one I cycle
}

template <u16 opcode, u16 roIndex>
inline void GBA_CPU::Thumb_LoadStoreRegisterOffset(u16 instruction)
{
    u16 rbIndex = ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = ExtractBits<2, 0, u16>(instruction);

    u32 ro = ReadRegister(roIndex);
    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    u32 address =  rb + ro;

    switch (opcode)
    {
        case 0: // STR
        {
            Write32(address, rd);
            break;
        }

        case 1: // STRB
        {
            Write8(address, rd & 0xFF)
            break;
        }

        case 2: // LDR
        {
            cpuState.registers[rdIndex] = Read32_Rotated(address);
            // TODO: Add one I cycle
            break;
        }

        case 3: // LDRB
        {
            cpuState.registers[rdIndex] = Read8(address);
            // TODO: Add one I cycle
            break;
        }
    }

    // TODO: Affect pipeline
    AdvanceProgramCounter();
}

template <u16 opcode, u16 roIndex>
inline void GBA_CPU::Thumb_LoadStoreSignExtended(u16 instruction)
{
    u16 rbIndex = ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = ExtractBits<2, 0, u16>(instruction);

    u32 ro = ReadRegister(roIndex);
    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    u32 address = rb + ro;

    switch (opcode)
    {
        case 0: // STRH
        {
            Write16(address, rd);
            break;
        }

        case 1: // LDRSB
        {
            cpuState.registers[rdIndex] = Read8_SignExtended(address)
            // TODO: Add one I cycle
            break;
        }

        case 2: // LDRH
        {
            cpuState.registers[rdIndex] = Read32_Rotated(address);
            // TODO: Add one I cycle
            break;
        }

        case 3: // LDRSH
        {
            cpuState.registers[rdIndex] = Read16_SignExtended(address);
            // TODO: Add one I cycle
            break;
        }
    }

    // TODO: Affect pipeline
    AdvanceProgramCounter();
}

template <u16 opcode, u16 offset_5>
inline void GBA_CPU::Thumb_LoadStoreImmediateOffset(u16 instruction)
{
    u16 rbIndex = ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = ExtractBits<2, 0, u16>(instruction);

    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    if (opcode < 2) // Offset is 0-31 for byte and 0-124 for word
    {
        offset_5 *= 4; 
    }

    u32 address = rb + offset_5;

    switch (opcode)
    {
        case 0: // STR 
        {
            Write32(address, rd);
        }

        case 1: // LDR
        {
            cpuState.registers[rdIndex] = Read32_Rotated(address)
            // TODO: Add one I cycle
        }

        case 2: // STRB
        {
            Write8(address, rd);
        }

        case 3: // LDRB
        {
            cpuState.registers[rdIndex] = Read8(address);
            // TODO: Add one I cycle
        }
    }

    // TODO: Affect pipeline
    AdvanceProgramCounter();
}

template <bool load, u16 offset_5>
inline void GBA_CPU::Thumb_LoadStoreHalfword(u16 instruction)
{
    u16 rbIndex = ExtractBits<5, 3, u16>(instruction);
    u16 rdIndex = ExtractBits<2, 0, u16>(instruction);

    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    offset_5 *= 2; // Stepped by 2 because this function works with halfwords
    u32 address = rb + offset_5;

    if (load) // LDRH
    {
        cpuState.registers[rdIndex] = Read16_Rotated(address);
        // TODO: Add one I cycle
    }
    else // STRH
    {
        Write16(address, rd & 0xFFFF);
    }

    // TODO: Affect pipeline
    AdvanceProgramCounter();
}

template <bool load, u16 rdIndex>
inline void GBA_CPU::Thumb_LoadStoreSPRelative(u16 instruction)
{
    u32 rd = ReadRegister(rdIndex);
    u16 offset_8 = ExtractBits<7, 0, u16>(instruction) * 4; // Stepped by 4, word accesses

    u32 address = cpuState.r13 + offset_8;

    if (load) // STR
    {
        Write32(address, rd);
    }
    else // LDR
    {
        cpuState.registers[rdIndex] = Read32_Rotated(address);
        // TODO: Add one I cycle
    }

    // TODO: Affect pipeline
    AdvanceProgramCounter();
}

template <bool getSP, u16 rdIndex>
inline void GBA_CPU::Thumb_GetRelativeAddress(u16 instruction)
{
    u32 rd = ReadRegister(rdIndex);
    u16 offset_8 = ExtractBits<7, 0, u16>(instruction) * 4; // Stepped by 4, word accesses

    if (getSP) // Stack pointer
    {
        cpuState.registers[rdIndex] = ADD(cpuState.r13, offset_8, false);
    }
    else // Program counter
    {
        cpuState.registers[rdIndex] = ADD((cpuState.r15 & ~2), offset_8, false);
    }
    
    // TODO: Affect pipeline
    AdvanceProgramCounter();
}

template <bool sub>
inline void GBA_CPU::Thumb_AddOffsetToStackPointer(u16 instruction)
{
    u16 offset_7 = ExtractBits<6, 0, u16>(instruction) * 4; // Stepped by 4, word accesses
    cpuState.r13 = sub ? SUB(cpuState.r13, offset_7, false) : ADD(cpuState.r13, offset_7, false);

    // TODO: Affect pipeline
    AdvanceProgramCounter();
}

template <bool pop, bool bit_pc_lr>
inline void GBA_CPU::Thumb_PushPopRegisters(u16 instruction)
{
    u16 registerList = ExtractBits<7, 0, u16>(instruction);

    // TODO: Affect pipeline
    AdvanceProgramCounter();

    // Handle special case for empty register lists.
    if (registerList == 0 && !bit_pc_lr)
    {
        if (pop)
        {
            cpuState.r15 = Read32(cpuState.r13);
            // TODO: Flush Pipeline
            cpuState.r13 += 0x40;
        }
        else
        {
            cpuState.r13 -= 0x40;
            Write32(cpuState.r13, cpuState.r15)
        }
        return;
    }

    if (pop)
    {
        u32 startAddress = cpuState.r13;
        u32 endAddress = startAddress + 4 * (bit_pc_lr + NumberOfSetBitsIn(registerList));
        u32 address = startAddress;

        for (int i = 0; i < 8; i++) // R0 - R7
        {
            if (registerList & (1 << i))
            {
                cpuState.registers[i] = Read32(address);
                address += 4;
            }
        }

        if (bit_pc_lr)
        {
            cpuState.r15 = Read32(address) & ~1; // Halfword alignment
            address += 4;
            // TODO: Pipeline flush here
        }

        assert(endAddress == address);
        cpuState.r13 = endAddress;
        // TODO: Add one I cycle
    }
    else
    {
        u32 startAddress = cpuState.r13 - 4 * (bit_pc_lr + NumberOfSetBitsIn(registerList));
        u32 endAddress = cpuState.r13 - 4;
        u32 address = startAddress;

        for (int i = 0; i < 8; i++) // R0 - R7
        {
            if (registerList & (1 << i))
            {
                Write32(address, cpuState.registers[i])
                address += 4;
            }
        }

        if (bit_pc_lr)
        {
            Write32(address, cpuState.r14);
            address += 4;
        }

        assert(endAddress == address - 4);
        cpuState.r13 = startAddress;
    }
}

template <bool load, u16 rbIndex>
inline void GBA_CPU::Thumb_LoadStoreMultiple(u16 instruction)
{
    u16 registerList = ExtractBits<7, 0, u16>(instruction);
    u32 rb = ReadRegister(rbIndex);
    u32 count = NumberOfSetBitsIn(registerList);

    // TODO: Affect pipeline
    AdvanceProgramCounter();

    if (count == 0) // Empty register list
    {
        if (load)
        {
            cpuState.r15 = Read32(rb);
            // Flush Pipeline
        }
        else
        {
            Write32(rb, cpuState.r15);
        }
        cpuState.registers[rbIndex] += 0x40;
        return;
    }

    if (load)
    {
        u32 startAddress = rb;
        u32 endAddress = rb + (count * 4);
        u32 address = startAddress;

        for (int i = 0; i < 8; i++)
        {
            if (registerList & (1 << i))
            {
                WriteRegister(i, Read32(address));
                address += 4;
            }
        }

        assert(endAddress == address);

        // [ARMv4T specific] Store the address in the register only 
        // if the base register was not part of the list
        if (!(registerList & (1 << rbIndex)))
        {
            WriteRegister(rbIndex, address);
        }
        
        // TODO: Add one I cycle
    }
    else
    {
        u32 first = 0;

        u32 startAddress = rb;
        u32 endAddress = rb + (count * 4);
        u32 address = startAddress;

        // Get the index of the first register in the list
        for (int i = 0; i < 8; i++)
        {
            if (registerList & (1 << i))
            {
                first = i;
                break;
            }
        }

        // Calculate new base register value (not updated yet)
        u32 rb_new = endAddress;

        // Store first register, then update the base register's value (writeback)
        Write32(address, ReadRegister(first));
        WriteRegister(rbIndex, rb_new);
        address += 4;

        // Store the rest of the registers
        for (int i = first + 1; i < 8; i++)
        {
            if (registerList & (1 << i))
            {
                Write32(address, ReadRegister(i));
                address += 4;
            }
        }

        assert(endAddress == address);
    }
}

template <u16 condition>
inline void GBA_CPU::Thumb_ConditionalBranch(u16 instruction)
{
    if (condition == 0xE) // Undefined in THUMB, AL in ARM
    {
        return; // TODO: For now, should go into undefined mode I think.
    }

    if (ConditionPassed(static_cast<Condition>(condition)))
    {
        u8 offset_8 = ExtractBits<7, 0, u16>(instruction);
        s32 offset_32 = static_cast<s32>(SignExtend_8(offset_8) * 2); // Offset is in steps of two

        cpuState.r15 += offset_32;
        // TODO: Pipeline flush
    }
    else
    {
        // TODO: Affect pipeline
        AdvanceProgramCounter();
    }
}

inline void GBA_CPU::Thumb_SoftwareInterrupt(u16 instruction)
{
    // Save CPSR
    cpuState.spsr[BANK_SVC].value = cpuState.cpsr.value;

    // Switch to Supervisor Mode, switch to ARM mode and disable IRQs
    SwitchMode(Mode::SVC);
    cpuState.cpsr.fields.thumb = 0;
    cpuState.cpsr.fields.irq_disable = 1;

    // Save one instruction ahead of the fault address and jump to SVC exception vector
    // TODO: Check
    cpuState.r14 = cpuState.r15 - 2; // Only minus 2 because we want the instruction after the SWI
    cpuState.r15 = 0x08;
    // TODO: Pipeline flush (ARM)
}

inline void GBA_CPU::Thumb_UnconditionalBranch(u16 instruction)
{
    u16 offset_11 = ExtractBits<10, 0, u16>(instruction);
    u32 offset_32 = static_cast<u32>(offset_11);
    s32 finalOffset;

    if (offset_11 & 0x400)
    {
        offset_32 |= 0xFFFFF800;
    }

    finalOffset = static_cast<s32>(offset_32) * 2; // In steps of 2

    cpuState.r15 += finalOffset;
    // TODO: Pipeline flush
}

template <bool secondInstruction>
inline void GBA_CPU::Thumb_LongBranchWithLink(u16 instruction)
{
    u32 offset_11 = static_cast<u32>(ExtractBits<10, 0, u16>(instruction));

    if (secondInstruction)
    {
        // Get the PC of the currently executing instruction
        u32 currentPC = cpuState.r15 - 4;
        cpuState.r15 = cpuState.r14 + (offset_11 << 1);

        // Save the next instruction address (+2 in thumb)
        cpuState.r14 = (currentPC + 2) | 1;

        // TODO: Flush pipeline
    }
    else
    {
        // Sign extend
        if (offset_11 & 0x400)
        {
            offset_11 |= 0xFFFFF800;
        }

        cpuState.r14 = cpuState.r15 + (offset_11 << 12);

        // TODO: Affect pipeline
        AdvanceProgramCounter();
    }
}
