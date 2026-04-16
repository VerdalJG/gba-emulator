#include "Core/GBA_CPU.hpp"
#include "Core/Memory/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/Memory/GBA_WaitstateController.hpp"
#include "Core/GBA_Bus.hpp"
#include "Core/CPU/Shifts.hpp"
#include "Core/IO/GBA_IO_Helpers.hpp"

#include "Utils/Logger.hpp"
#include "Utils/BitOperations.hpp"

#include <assert.h>
#include <string>

GBA_CPU::GBA_CPU(EmulatorCore* core, GBA_Bus& bus) : core(core), bus(bus)
{
    assert(core && "CPU must have valid EmulatorCore object");
}

void GBA_CPU::Reset()
{
    if (skipBios)
    {
        cpuState.Reset();
        SwitchMode(Mode::SYS);
        cpuState.cpsr.fields.thumb = 0;
        cpuState.cpsr.fields.irq_disable = 0;
        cpuState.cpsr.fields.fiq_disable = 0;
        cpuState.r13 = 0x03007F00;
        cpuState.bankedR13_R14[BANK_SVC][BANK_R13] = 0x03007FE0;
        cpuState.bankedR13_R14[BANK_IRQ][BANK_R13] = 0x03007FA0;
        cpuState.r15 = 0x08000000;
        FlushPipeline();
        Write16(0x04000000, 0, 0); // Set DISPCNT to 0
        return;
    }
    
    // Reset all registers and set CPSR to it's reset value (0x000000D3)
    cpuState.Reset();
    SwitchMode(Mode::SVC);
    FlushPipeline();

    globalCycles = 0;
    cycles = 0;
}

void GBA_CPU::Step()
{
    if (halted)
    {
        HandleHalt();
        return; // No instructions/cycles during cpu halt
    }

    cycles = 0;

    AdvanceInstructionPipeline();

    Fetch();
    Decode();
    Execute(); // Instructions advance the program counter if appropriate
}

void GBA_CPU::RequestInterrupt()
{

}

void GBA_CPU::RestoreCPSRFromSPSR(ExceptionBank bankIndex)
{
    cpuState.cpsr = cpuState.spsr[bankIndex];
}

void GBA_CPU::SaveCPSRIntoSPSR(ExceptionBank bankIndex)
{
    cpuState.spsr[bankIndex] = cpuState.cpsr;
}

ExceptionBank GBA_CPU::GetBankFromMode(Mode mode)
{
    switch(mode)
    {
        case Mode::FIQ: return BANK_FIQ;
        case Mode::IRQ: return BANK_IRQ;
        case Mode::SVC: return BANK_SVC;
        case Mode::ABT: return BANK_ABT;
        case Mode::UND: return BANK_UND;
        default: return BANK_UNBANKED;
    }
}

StatusRegister GBA_CPU::GetCurrentSPSR()
{
    // TODO: maybe has specific behavior with LDM usermode conflict
    return StatusRegister{currentSPSR->value}; // CurrentSPSR pointer is always set, no nullcheck needed
}

void GBA_CPU::SwitchMode(Mode newMode)
{
    Mode oldMode = cpuState.cpsr.fields.mode;

    ExceptionBank oldBank = GetBankFromMode(oldMode);
    ExceptionBank newBank = GetBankFromMode(newMode);
    
    cpuState.cpsr.fields.mode = newMode;

    // In USR/SYS mode reading SPSR returns CPSR
    if (newBank == BANK_UNBANKED)
    {
        currentSPSR = &cpuState.cpsr;
    }
    else // Banked mode
    {
        currentSPSR = &cpuState.spsr[newBank];
    }

    if (oldBank == newBank) return;

    // Manage R8 - R12 (only used by USR, SYS, FIQ)
    if (oldBank == BANK_FIQ)
    {
        // Save to FIQ bank
        for (int i = 0; i < FIQ_RCOUNT; i++)
        {
            cpuState.fiq_banked_R8_R12[i] = cpuState.registers[i + 8];
        }
        
        // Load the 'USR bank'
        for (int i = 0; i < FIQ_RCOUNT; i++)
        {
            cpuState.registers[i + 8] = cpuState.shared_R8_R12[i];
        }
    }
    else if (newBank == BANK_FIQ)
    {
        // Save to USR bank
        for (int i = 0; i < FIQ_RCOUNT; i++)
        {
            cpuState.shared_R8_R12[i] = cpuState.registers[i + 8];
        }

        // Load the FIQ bank
        for (int i = 0; i < FIQ_RCOUNT; i++)
        {
            cpuState.registers[i + 8] = cpuState.fiq_banked_R8_R12[i];
        }
    }

    // Store the current R13 and R14 inside the bank of the respective mode
    cpuState.bankedR13_R14[oldBank][BANK_R13] = cpuState.r13;
    cpuState.bankedR13_R14[oldBank][BANK_R14] = cpuState.r14;

    // Load the new bank R13/14
    cpuState.r13 = cpuState.bankedR13_R14[newBank][BANK_R13];
    cpuState.r14 = cpuState.bankedR13_R14[newBank][BANK_R14];
}

void GBA_CPU::HandleHalt()
{
    if (/*interrupts.AnyPendingEnabled(*this)*/ true)
    {
        halted = false; 
    }
    else
    {
        // Advance timers
        // Handle DMA
    }
}

void GBA_CPU:: AdvanceInstructionPipeline()
{
    pipeline.stage[2] = pipeline.stage[1]; // Move decoded instruction [1] to execute stage [2];
    pipeline.stage[1] = pipeline.stage[0]; // Move fetch instruction [0] to decode stage [1];
}

void GBA_CPU::FlushPipeline()
{
    pipeline.stage[0].valid = false;
    pipeline.stage[1].valid = false;
    pipeline.stage[2].valid = false;
    pipeline.access = Access::Code | Access::Nonsequential;   
}

void GBA_CPU::Fetch()
{
    u32 address = cpuState.r15 & (IsThumbMode() ? ~1u : ~3u);
    u32 fetched = IsThumbMode() ? Read16(address, pipeline.access) : Read32(address, pipeline.access);
    pipeline.stage[0] = { fetched, ARM_Suppressed, true };
    pipeline.access = Access::Code | Access::Sequential;
}

void GBA_CPU::Decode()
{
    if (!pipeline.stage[1].valid) return;

    u32 instructionBits = pipeline.stage[1].rawBits;
    
    if (IsThumbMode()) 
    {
        pipeline.stage[1].opcode = Decode_Thumb(instructionBits);
    }
    else // ARM Mode
    {
        pipeline.stage[1].opcode = Decode_ARM(instructionBits);
    }
}

void GBA_CPU::Execute()
{
    if (!pipeline.stage[2].valid) // No-op // 0x080016ec vblank? // 0x0800179c test failed after first swi?
    {
        AdvanceProgramCounter();
        return;
    }

    // Check condition for ARM
    if (!IsThumbMode())
    {
        Condition condition = GetConditionType(pipeline.stage[2].rawBits);

        if (condition == CONDITION_NV || !ConditionPassed(condition)) // Unpredictable, treat as no-op
        {
            pipeline.stage[2].opcode = ARM_Opcode::ARM_Suppressed;
        }

        if (pipeline.stage[2].opcode == ARM_Opcode::ARM_Suppressed) // Condition failed, no-op
        {
            AdvanceProgramCounter();
            return;
        }
    }
    else // In theory thumb 16 bits always encodes to an opcode, invalid can never happen but we log just in case
    {
        if (pipeline.stage[2].opcode == Thumb_Opcode::Thumb_Invalid)
        {
            Log("Thumb invalid instruction", LogType::Error);
            AdvanceProgramCounter();
            return;
        }
    }

    if (logInstructions)
    {
        std::string pc = "PC is at:" + IntToHex(cpuState.r15);
        std::string bits = ", Bits= " + IntToHex(pipeline.stage[2].rawBits);
        std::string opcode = ", Opcode= " + (IsThumbMode() ? 
        ThumbOpToString(pipeline.stage[2].opcode) : 
        ArmOpToString(pipeline.stage[2].opcode));
        
        std::string message = pc + bits + opcode;

        Log(message, LogType::Info);
    }

    // Execute the instruction
    if (IsThumbMode())
    {
        Thumb_Handler function = thumbDispatchTable[pipeline.stage[2].opcode];
        (this->*function)(pipeline.stage[2].rawBits);
    }
    else
    {
        ARM_Handler function = armDispatchTable[pipeline.stage[2].opcode];
        (this->*function)(pipeline.stage[2].rawBits);
    }

    
}

bool GBA_CPU::ConditionPassed(Condition condition)
{
    uint conditionBits = (cpuState.cpsr.value >> 28);
    return conditionTable[(static_cast<int>(condition) << 4) | conditionBits];
}

void GBA_CPU::AddCycles(u32 cycles)
{
    this->cycles += cycles;
    globalCycles += cycles;
}

EmulatorCore* GBA_CPU::GetCore()
{
    return core;
}

void GBA_CPU::Log(const std::string& message, LogType logType, const std::string functionName)
{
    if (core)
    {
       core->Log(message, logType, functionName);
    }
}

u32 GBA_CPU::Read8(u32 address, uint access)
{
    return bus.Read<u8>(address, BusRequester::CPU);
}

u32 GBA_CPU::Read16(u32 address, uint access)
{
    return bus.Read<u16>(address, BusRequester::CPU);
}

u32 GBA_CPU::Read32(u32 address, uint access)
{
    return bus.Read<u32>(address, BusRequester::CPU);
}

void GBA_CPU::Write8(u32 address, u8 value, uint access)
{
    bus.Write<u8>(address, value, BusRequester::CPU);
}

void GBA_CPU::Write16(u32 address, u16 value, uint access)
{
    bus.Write<u16>(address, value, BusRequester::CPU);
}

void GBA_CPU::Write32(u32 address, u32 value, uint access)
{
    bus.Write<u32>(address, value, BusRequester::CPU);
}

u32 GBA_CPU::Read16_Rotated(u32 address, uint access) 
{
    u32 value = bus.Read<u16>(address, BusRequester::CPU);
    uint carry = GetCPSR_C(); // Used as dummy value

    if (address & 1) // ROR 8 if misaligned
    {
        ROR(value, 8, carry, false);
    }

    return value;
}

u32 GBA_CPU::Read32_Rotated(u32 address, uint access) 
{
    u32 value = bus.Read<u32>(address, BusRequester::CPU);
    u32 shift = (address & 3) * 8;
    uint carry = GetCPSR_C(); // Used as dummy value

    ROR(value, shift, carry, false); // Rotate based on alignment

    return value;
}

u32 GBA_CPU::Read8_Signed(u32 address, uint access) 
{ 
    u8 value = bus.Read<u8>(address, BusRequester::CPU);
    return SignExtend_8(value); 
}

u32 GBA_CPU::Read16_Signed(u32 address, uint access) 
{ 
    if (address & 1) // Misaligned
    {
        u8 value = bus.Read<u8>(address, BusRequester::CPU);
        return SignExtend_8(value);
    }
    else
    {
        u16 value = bus.Read<u16>(address, BusRequester::CPU);
        return SignExtend_16(value);
    }
}

void GBA_CPU::InvalidateSequentiality() 
{
    bus.InvalidateSequentiality();
}

/*
64 - pc at 0x70, z flag set to 0
65 - r14 is now 0x4
66 - r12 is now 0x04000000
67 - r12 is now 0x80
68 - r12 is not eq 1, nzcv = 0
69 - mrseq, does not pass
70 - orreq, does not pass
71 - msreq, does not pass
72 - beq, does not pass
73 - r0 is 0xdf, df is 1101 1111
74 - put r0 into cpsr, switch to SYS from SVC, bank r14_SVC
75 - r4 is now 0x04000000
76 - store 0 into IME io register
77 - BRANCH AND LINK PC IS NOW 000000E0, r14_SYS = 0xa0

PC is 0x000000E0
99 - r0 is 0xd3
100 - put r0 into cpsr, switch to SVC from SYS, bank r14_SYS(0xa0), load r14_SVC
101 - load r13, r13 is now 0x03007fe0
102 - r14 is now 0
103 - spsr_SVC = 0
104 - r0 is now 0xd2
105 - Switch to IRQ mode, bank r13_SVC(0x03007fe0)
106 - Set r13_IRQ to 0x03007FA0
107 - lr is now 0 
108 - SPSR_IRQ = 0 now
109 - r0 = 0x5f
110 - cpsr = 0x5f, 0101 1111 (IRQ enable), switch back to SYS mode, r13_IRQ banked
111 - r13_SYS = 0x03007F00
113 - r0 = 0x0000011D (0x0000011C + 1)
114 - bx to r0 and switch to thumb

PC is 0x0000011C
117 - r0 = 0 and cpsr flags set (z = 1, all others = 0)
118 - r1 = 0xFFFFFE00

PC is 0x00000120 (512 bytes clearing in IWRAM loop)
120 - Store r0 (0) at [IO_Region + r1 (0xFFFFFe00)] = at 0x03FFFE00 (IWRAM)
123 - add r1 +=4 and set flags (increment until r1 overflows)
124 - branch if n != v, loop back to 0x00000120 
125 - once loop is complete, BX (branch&exchange) to r14 (0xa0 - line 78)
c and z flag is set after loop, switch back to ARM mode

PC is 0x000000A0
78 - r0 = PC(0xA8) + 0x258 (add) = 0x300
79 - store 0x300 at [r13(0x03007F00) + 0xfc] = 0x03007FFC
80 - load [address = 0x0000027C] (value = 0x00001929) into r0
81 - r14_SYS = PC (0xb4 + 0)
82 - BX to r0 (0x1929) (switch to thumb)

PC is 0x00001928 (halfword align) - LINE 1194
B5F0 - push (STM) {r4-r7, lr}, r13_SYS = 0x03007EEC (TODO: Maybe advancing PC early)
B08D - sub sp #52
???? - mov r1, #0
cpuState.r15 == 0x1928
*/

/*

macro m_test_init 
{
        m_text_init
        {
            stmfd   sp!, {r0-r1, lr}
            mov     r0, 4                   ; Background mode 4
            orr     r0, 1 shl 10            ; Background 2
            mov     r1, MEM_IO
            strh    r0, [r1, REG_DISPCNT]
            ldmfd   sp!, {r0-r1, pc}
        }

        m_text_color 0xFFFF, 0
        {
            m_half  r0, color
            {
                mov     reg (r0), color (0xFFFF) and 0xFF
                orr     reg (r0), color (0xFFFF) and 0xFF00
            }
            mov     r1, index (0)
            bl      text_color
            {
                ; r0:   color
                ; r1:   index
                stmfd   sp!, {r0-r2, lr}
                lsl     r1, 1
                mov     r2, MEM_PALETTE
                strh    r0, [r2, r1]
                ldmfd   sp!, {r0-r2, pc}
            }
        }

        m_text_color 0x0000, 1
        {
            m_half  r0, color
            {
                mov     reg (r0), color (0x0000) and 0xFF
                orr     reg (r0), color (0x0000) and 0xFF00
            }
            mov     r1, index (1)
            bl      text_color
            {
                ; r0:   color
                ; r1:   index
                stmfd   sp!, {r0-r2, lr}
                lsl     r1, 1
                mov     r2, MEM_PALETTE
                strh    r0, [r2, r1]
                ldmfd   sp!, {r0-r2, pc}
            }
        }

        m_text_color 0xFFFF, 2
        {
            m_half  r0, color
            {
                mov     reg (r0), color (0xFFFF) and 0xFF
                orr     reg (r0), color (0xFFFF) and 0xFF00
            }
            mov     r1, index (2)
            bl      text_color
            {
                ; r0:   color
                ; r1:   index
                stmfd   sp!, {r0-r2, lr}
                lsl     r1, 1
                mov     r2, MEM_PALETTE
                strh    r0, [r2, r1]
                ldmfd   sp!, {r0-r2, pc}
            }
        }
}

swi at failed test line 95: pc == 0x0800179c
0x08001798 is return address from swi
0x190 is return from exception handler line 199 bios
0x080017a8 is return address from second swi

cpuState.r15 == 0x3bc first instruction of DIV routine

cpuState.r15 == 0x3c4 eors ip line 406

test 104 fails

DIV routine is now fine, at 0x080017b8 - m_text_pos 60, 76, line 102 of macros.inc

0x080017c0 m_text char first in failed tests

0x080016d0 is right after branching to eval

0x080016f0 is right after branching to m_test_eval (stmfd sp!, \{r0-r12\}), line 66 in macros.inc

[INFO]: PC is at:080016F0, Bits= E8BD0003, Opcode= ARM_BlockDataTransfer
[INFO]: PC is at:080016F4, Bits= E92D1FFF, Opcode= ARM_BlockDataTransfer
[INFO]: PC is at:080016F8, Bits= E1B0C00C, Opcode= ARM_DataProcessing
[INFO]: PC is at:08001700, Bits= 1A000022, Opcode= ARM_Branch
[INFO]: PC is at:08001790, Bits= E3A0B403, Opcode= ARM_DataProcessing
[INFO]: PC is at:08001794, Bits= E1A0000C, Opcode= ARM_DataProcessing
[INFO]: PC is at:08001798, Bits= E3A01064, Opcode= ARM_DataProcessing
[INFO]: PC is at:0800179C, Bits= EF060000, Opcode= ARM_SoftwareInterrupt

[INFO]: PC is at:00000010, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000078, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000000E0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000148, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000001B0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000218, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000280, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000002E8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000350, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000003B8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000420, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000488, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000004F0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000558, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000005C0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000628, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000690, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000006F8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000760, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000007C8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000830, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000898, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000900, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000968, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000009D0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000A38, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000AA0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000B08, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000B70, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000BD8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000C40, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000CA8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000D10, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000D78, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000DE0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000E48, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000EB0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000F18, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000F80, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00000FE8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001050, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000010B8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001120, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001188, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000011F0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001258, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000012C0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001328, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001390, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000013F8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001460, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000014C8, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001530, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001598, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001600, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001668, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000016D0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001738, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:000017A0, Bits= EA000018, Opcode= ARM_Branch
[INFO]: PC is at:00001808, Bits= EA000018, Opcode= ARM_Branch
*/