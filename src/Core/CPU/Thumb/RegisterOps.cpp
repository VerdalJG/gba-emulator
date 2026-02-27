#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Shifts.hpp"
#include "Core/CPU/MultiplyTiming.hpp"

#include "Utils/BitOperations.hpp"


void GBA_CPU::Thumb_MoveShiftedRegister(u16 instruction)
{
    const u16 shiftOp = ExtractBits<12, 11>(instruction);
    const u16 offset_5 = (instruction >> 6) & 0x1F;

    const u16 rsIndex = ExtractBits<5, 3>(instruction);
    const u16 rdIndex = ExtractBits<2, 0>(instruction);

    u32 rs = cpuState.registers[rsIndex];

    u32 carry = GetCPSR_C();
    ApplyShift(shiftOp, rs, offset_5, carry, true);

    // Update CPSR flags
    UpdateNZFlags(rs);
    cpuState.cpsr.fields.c = carry;

    cpuState.registers[rdIndex] = rs;
    pipeline.access = Access::Code | Access::Sequential;
    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_AddSubtract(u16 instruction)
{
    const bool immediate = IsBitSet<10>(instruction);
    const bool subtract  = IsBitSet<9>(instruction);
    const u16 operand = ExtractBits<8, 6>(instruction);

    const u16 rsIndex = ExtractBits<5, 3>(instruction);
    const u16 rdIndex = ExtractBits<2, 0>(instruction);

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

    pipeline.access = Access::Code | Access::Sequential;
    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_ImmediateOp(u16 instruction)
{
    const u16 opcode = ExtractBits<12, 11>(instruction);
    const u16 rdIndex = ExtractBits<10, 8>(instruction);

    const u16 immediate_8 = ExtractBits<7, 0>(instruction);

    switch (opcode)
    {
        case 0b00: // MOV
            cpuState.registers[rdIndex] = immediate_8;
            UpdateNZFlags(0);

        case 0b01: // CMP
            SUB(cpuState.registers[rdIndex], immediate_8, true);
        
        case 0b10: // ADD
            cpuState.registers[rdIndex] = ADD(cpuState.registers[rdIndex], immediate_8, true);

        case 0b11: // SUB
            cpuState.registers[rdIndex] = SUB(cpuState.registers[rdIndex], immediate_8, true);
    }

    pipeline.access = Access::Code | Access::Sequential;
    AdvanceProgramCounter();
}

inline void GBA_CPU::Thumb_ALU(u16 instruction)
{
    const u16 opcode = ExtractBits<9, 6>(instruction);

    const u16 rsIndex = ExtractBits<5, 3>(instruction);
    const u16 rdIndex = ExtractBits<2, 0>(instruction);

    u32 rs = cpuState.registers[rsIndex];
    u32 rd = cpuState.registers[rdIndex];

    u32 result;

    switch (static_cast<Thumb_ALUOp>(opcode))
    {
        case Thumb_ALUOp::AND:
        {
            result = rd & rs;
            cpuState.registers[rdIndex] = result;
            UpdateNZFlags(result);
            break;
        }            
        
        case Thumb_ALUOp::EOR:
        {
            result = rd ^ rs;
            cpuState.registers[rdIndex] = rd ^ rs;
            UpdateNZFlags(result);
            break;
        }
        
        case Thumb_ALUOp::LSL:
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

        case Thumb_ALUOp::LSR:
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
            
        case Thumb_ALUOp::ASR:
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
            
        case Thumb_ALUOp::ADC:
        {
            rd = ADC(rd, rs, true); // CPSR flags handled inside ADC()
            break;
        }
            
        case Thumb_ALUOp::SBC:
        {
            rd = SBC(rd, rs, true); // CPSR flags handled inside SBC()
            break;
        }
            
        case Thumb_ALUOp::ROR:
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
            
        case Thumb_ALUOp::TST:
        {
            UpdateNZFlags(rd & rs);
            break;
        }
            
        case Thumb_ALUOp::NEG:
        {
            cpuState.registers[rdIndex] = SUB(0, rs, true); // CPSR flags handled inside SUB()
            break;
        }
            
        case Thumb_ALUOp::CMP:
        {
            SUB(rd, rs, true); // CPSR flags handled inside SUB()
            break;
        }
            
        case Thumb_ALUOp::CMN:
        {
            ADD(rd, rs, true); // CPSR flags handled inside ADD()
            break;
        }
            
        case Thumb_ALUOp::ORR:
        {
            result = rd | rs;
            cpuState.registers[rdIndex] = result;
            UpdateNZFlags(result);
            break;
        }
            
        case Thumb_ALUOp::MUL: 
        {
            result = rd * rs;
            cpuState.registers[rdIndex] = result;
            UpdateNZFlags(result);

            u32 cycles = CalculateMultiplierCycles(rs);
            // TODO: Add mI cycles properly
            break;
        }
            
        case Thumb_ALUOp::BIC:
        {
            result = rd & ~rs;
            cpuState.registers[rdIndex] = result;
            UpdateNZFlags(result);
            break;
        }
            
        case Thumb_ALUOp::MVN:
        {
            cpuState.registers[rdIndex] = ~rs;
            UpdateNZFlags(~rs);
            break;
        }
    }

    pipeline.access = Access::Code | Access::Sequential;
    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_HiRegisterOp(u16 instruction)
{
    const u16 opcode = ExtractBits<9, 8>(instruction);
    const u16 msbRd = IsBitSet<7>(instruction);
    const u16 msbRs = IsBitSet<6>(instruction);

    // Restriction: For ADD/CMP/MOV, MSBs and/or MSBd must be set
    if (opcode < 3 && msbRs == 0 && msbRd == 0)
    {
        pipeline.access = Access::Code | Access::Sequential;
        AdvanceProgramCounter();
        return; 
    }

    // MSB allows for access to hi registers
    const u16 rsIndex = (msbRs << 4) | ExtractBits<5, 3>(instruction);
    const u16 rdIndex = (msbRd << 4) | ExtractBits<2, 0>(instruction);

    u32 rs = cpuState.registers[rsIndex];
    u32 rd = cpuState.registers[rdIndex];
    
    switch (opcode)
    {
        case 0: // ADD
        {
            cpuState.registers[rdIndex] = rd + rs;
            if (rdIndex == 15)
            {
                cpuState.r15 &= ~1; // Halfword align
                FlushPipeline();
            }
            else
            {
                pipeline.access = Access::Code | Access::Sequential;
                AdvanceProgramCounter();
            }
            break;
        }

        case 1: // CMP
        {
            SUB(rd, rs, true);
            pipeline.access = Access::Code | Access::Sequential;
            AdvanceProgramCounter();
            break;
        }
        
        case 2: //MOV
        {
            cpuState.registers[rdIndex] = rs;
            if (rdIndex == 15)
            {
                cpuState.r15 &= ~1; // Halfword align
                FlushPipeline();
            }
            else
            {
                pipeline.access = Access::Code | Access::Sequential;
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
                FlushPipeline();
            }
            else // Switch to ARM
            {
                cpuState.cpsr.fields.thumb = 0;
                cpuState.r15 = rs & ~2; // Bit 1 is already cleared, this makes it word-aligned
                FlushPipeline();
            }
            break;
        }
    }
}