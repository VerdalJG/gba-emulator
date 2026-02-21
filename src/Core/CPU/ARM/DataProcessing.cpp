#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Shifts.hpp"
#include "Core/CPU/InstructionHelpers.hpp"

#include "Utils/BitOperations.hpp"


void GBA_CPU::ARM_DataProcessing(u32 instruction)
{
    const bool immediate = IsBitSet<25>(instruction);
    const ARM_ALUOp opcode = static_cast<ARM_ALUOp>(ExtractBits<24, 21>(instruction));
    const bool set_flags = IsBitSet<20>(instruction);

    const u32 rnIndex = ExtractBits<19, 16>(instruction);
    const u32 rdIndex = ExtractBits<15, 12>(instruction);

    u32 rn = ReadRegister(rnIndex);
    bool rd_isPC = rdIndex == 15;

    const bool shiftByImmediate = IsBitSet<4>(instruction);

    uint carry = GetCPSR_C();
    u32 op1;
    u32 op2;

    pipeline.access = Access::Code | Access::Sequential;

    if (immediate) // Immediate Op2
    {
        u32 shiftAmount = ExtractBits<11, 8>(instruction) * 2; // 0 - 30, in steps of 2
        u32 immediate_8 = ExtractBits<7, 0>(instruction);

        ROR(immediate_8, shiftAmount, carry, immediate);

        op1 = rn;
        op2 = immediate_8;
    }
    else // Register Op2
    {
        const uint shiftOp = ExtractBits<6, 5>(instruction);

        const u32 rmIndex = ExtractBits<3, 0>(instruction);
        u32 rm = ReadRegister(rmIndex);

        u32 shift;

        if (shiftByImmediate)
        {
            shift = ExtractBits<11, 7>(instruction);
        }
        else // Shift by register
        {
            u32 shiftRegisterIndex = ExtractBits<11, 8>(instruction);
            shift = ReadRegister(shiftRegisterIndex) & 0xFF; // only lower byte used

            AdvanceProgramCounter();

            // TODO: Add one I cycle

            pipeline.access = Access::Code | Access::Nonsequential;
        }

        ApplyShift(shiftOp, rm, shift, carry, immediate);
        op1 = rn;
        op2 = rm;
    }

    StatusRegister& cpsr = cpuState.cpsr;
    u32 result;

    switch (opcode)
    {
        case ARM_ALUOp::AND:
        {
            result = op1 & op2;
            cpuState.registers[rdIndex] = result;
            if (set_flags && !rd_isPC)
            {
                UpdateNZFlags(result);
                cpsr.fields.c = carry;
            }
            break;
        }

        case ARM_ALUOp::EOR:
        {
            result = op1 ^ op2;
            cpuState.registers[rdIndex] = result;
            if (set_flags && !rd_isPC)
            {
                UpdateNZFlags(result);
                cpsr.fields.c = carry;
            }
            break;
        }

        case ARM_ALUOp::SUB:
        {
            cpuState.registers[rdIndex] = SUB(op1, op2, set_flags && !rd_isPC);
            break;
        }

        case ARM_ALUOp::RSB:
        {
            cpuState.registers[rdIndex] = SUB(op2, op1, set_flags && !rd_isPC);
            break;
        }

        case ARM_ALUOp::ADD:
        {
            cpuState.registers[rdIndex] = ADD(op1, op2, set_flags && !rd_isPC);
            break;
        }

        case ARM_ALUOp::ADC:
        {
            cpuState.registers[rdIndex] = ADC(op1, op2, set_flags && !rd_isPC);
            break;
        }

        case ARM_ALUOp::SBC:
        {
            cpuState.registers[rdIndex] = SBC(op1, op2, set_flags && !rd_isPC);
            break;
        }

        case ARM_ALUOp::RSC:
        {
            cpuState.registers[rdIndex] = SBC(op2, op1, set_flags && !rd_isPC);
            break;
        }

        case ARM_ALUOp::TST:
        {
            UpdateNZFlags(op1 & op2);
            cpsr.fields.c = carry;
            break;
        }

        case ARM_ALUOp::TEQ:
        {
            UpdateNZFlags(op1 ^ op2);
            cpsr.fields.c = carry;
            break;
        }

        case ARM_ALUOp::CMP:
        {
            SUB(op1, op2, true);
            break;
        }

        case ARM_ALUOp::CMN:
        {
            ADD(op1, op2, true);
            break;
        }

        case ARM_ALUOp::ORR:
        {
            result = op1 | op2;
            cpuState.registers[rdIndex] = result;
            if (set_flags && !rd_isPC)
            {
                UpdateNZFlags(result);
                cpsr.fields.c = carry;
            }
            break;
        }

        case ARM_ALUOp::MOV:
        {
            cpuState.registers[rdIndex] = op2;
            if (set_flags && !rd_isPC)
            {
                UpdateNZFlags(op2);
                cpsr.fields.c = carry;
            }
            break;
        }

        case ARM_ALUOp::BIC:
        {
            result = op1 & ~op2;
            cpuState.registers[rdIndex] = result;
            if (set_flags && !rd_isPC)
            {
                UpdateNZFlags(result);
                cpsr.fields.c = carry;
            }
            break;
        }

        case ARM_ALUOp::MVN:
        {
            cpuState.registers[rdIndex] = ~op2;
            if (set_flags && !rd_isPC)
            {
                UpdateNZFlags(~op2);
                cpsr.fields.c = carry;
            }
            break;
        }
    }

    if (rd_isPC)
    {
        if (set_flags)
        {
            StatusRegister spsr = GetCurrentSPSR();

            SwitchMode(spsr.fields.mode);
            cpuState.cpsr.value = spsr.value;
        }

        // Flush pipeline if R15 is changed. Luckily the 4 opcodes that do not update Rd are grouped
        if (opcode >= ARM_ALUOp::TST && opcode <= ARM_ALUOp::CMN)
        {
            FlushPipeline();
        }
        else if (immediate || shiftByImmediate) 
        {
            AdvanceProgramCounter();
        }
    }
    else if (immediate || shiftByImmediate)
    {
        AdvanceProgramCounter();
    }

}