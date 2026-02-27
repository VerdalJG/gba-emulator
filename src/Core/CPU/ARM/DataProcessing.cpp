#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Shifts.hpp"

#include "Utils/BitOperations.hpp"

#include <assert.h>

void GBA_CPU::ARM_DataProcessing(u32 instruction)
{
    const bool immediate = IsBitSet<25>(instruction);
    const ARM_ALUOp opcode = static_cast<ARM_ALUOp>(ExtractBits<24, 21>(instruction));
    const bool set_flags = IsBitSet<20>(instruction);

    const u32 rnIndex = ExtractBits<19, 16>(instruction);
    const u32 rdIndex = ExtractBits<15, 12>(instruction);

    u32 rn = ReadRegister(rnIndex);
    bool rd_isPC = rdIndex == 15;

    const bool shiftByImmediate = !IsBitSet<4>(instruction);

    uint carry = GetCPSR_C();
    u32 op1;
    u32 op2;

    pipeline.access = Access::Code | Access::Sequential;

    if (immediate) // Immediate Op2
    {
        u32 shiftAmount = ExtractBits<11, 8>(instruction) * 2; // 0 - 30, in steps of 2
        u32 immediate_8 = ExtractBits<7, 0>(instruction);

        ROR(immediate_8, shiftAmount, carry, false); // RRX only happens in register shifted by immediate encoding

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

        ApplyShift(shiftOp, rm, shift, carry, shiftByImmediate);
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

void GBA_CPU::ARM_PSRTransfer(u32 instruction)
{
    bool useSPSR = IsBitSet<22>(instruction);
    bool toStatusRegister = IsBitSet<21>(instruction);
    Mode currentMode = GetCurrentMode();

    if (useSPSR && !CurrentModeHasSPSR())
    {
        // UNPREDICTABLE, ignore for now
    }

    StatusRegister& psr = useSPSR ? *currentSPSR : cpuState.cpsr; // Current SPSR can never be nullptr
   
    if (toStatusRegister) // MSR - Move to status register from ARM register
    {
        bool immediate = IsBitSet<25>(instruction);
        bool writeFlags = IsBitSet<19>(instruction);
        bool writeStatus = IsBitSet<18>(instruction);
        bool writeExtension = IsBitSet<17>(instruction);
        bool writeControl = IsBitSet<16>(instruction);
        u32 operand;

        if (immediate)
        {
            u32 shift = ExtractBits<11, 8>(instruction) * 2; // In steps of 2
            u32 immediate_8 = ExtractBits<7, 0>(instruction);
            u32 carry = cpuState.cpsr.fields.c;
            ROR(immediate_8, shift, carry, false); // False to prevent RRX #1 on ROR #0, carry is unused anyways
            operand = immediate_8;
        }
        else
        {
            u32 rmIndex = ExtractBits<3, 0>(instruction);
            operand = ReadRegister(rmIndex);
        }

        u32 mask = 0;

        if (writeControl)   mask |= 0xFF;
        if (writeExtension) mask |= 0xFF << 8;
        if (writeStatus)    mask |= 0xFF << 16;
        if (writeFlags)     mask |= 0xFF << 24;

        if (!useSPSR && currentMode == Mode::USR)
        {
            mask &= 0xFF000000; // allow flags only
        }

        // Clear respective bits then OR them with the operand's respective bits (for new value)
        u32 oldValue = psr.value;
        u32 newValue = (oldValue & ~mask) | (operand & mask);

        if (!useSPSR)
        {
            newValue |= 0x10; // Bit 4 is forced to be set on ARM7TDMI

            Mode oldMode = psr.fields.mode;
            Mode newMode = static_cast<Mode>(newValue & 0x1F);

            if (oldMode != newMode)
            {
                SwitchMode(newMode);
            }

            psr.value = newValue;
        }
        else
        {
            psr.value = newValue;
        }
    }
    else // MRS - Move PSR to General purpose register
    {
        u32 rdIndex = ExtractBits<15, 12>(instruction);
        assert(rdIndex != 15 && "Rd cannot be 15 for MRS");
        cpuState.registers[rdIndex] = psr.value;
    }

    AdvanceProgramCounter();
}
