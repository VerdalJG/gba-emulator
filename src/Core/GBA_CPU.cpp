#include "Core/GBA_CPU.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_WaitstateController.hpp"
#include "Core/GBA_Bus.hpp"
#include "Core/CPU/Instructions/ARM/Conditions.hpp"
#include "Core/CPU/Shifts.hpp"
#include "Utils/BitOperations.hpp"

#include <assert.h>

GBA_CPU::GBA_CPU(EmulatorCore* core, GBA_Bus& bus) : core(core), bus(bus)
{
    assert(core && "CPU must have valid EmulatorCore object");
}

void GBA_CPU::Reset()
{
    // Reset all registers and set CPSR to it's reset value (0x000000D3)
    cpuState.Reset();
    FlushPipeline();

    //totalCycles = 0;
}

void GBA_CPU::Step()
{
    if (halted)
    {
        HandleHalt();
        return; // No instructions/cycles during cpu halt
    }

    //currentInstructionCycles = 0;

    AdvanceInstructionPipeline();

    Fetch();
    Decode();
    Execute(); // Instructions advance the program counter if appropriate
}

void GBA_CPU::RequestInterrupt()
{

}

void GBA_CPU::RestoreCPSRFromSPSR(ExceptionBank oldExceptionModeIndex)
{
    cpuState.cpsr = cpuState.spsr[oldExceptionModeIndex];
}

void GBA_CPU::SaveCPSRIntoSPSR(ExceptionBank exceptionModeIndex)
{
    cpuState.spsr[exceptionModeIndex] = cpuState.cpsr;
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
    u32 address = cpuState.r15;
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
        // Thumb mode does not use condition bits
        pipeline.stage[1].opcode = Decode_Thumb(instructionBits);
    }
    else // ARM Mode
    {
        Condition condition = GetConditionType(pipeline.stage[1].rawBits);
        if (condition == CONDITION_NV) return; // Unpredictable, treat as no-op

        if (ConditionPassed(condition))
        {
            pipeline.stage[1].opcode = Decode_ARM(instructionBits);
        }
    }
}

void GBA_CPU::Execute()
{
    if (!pipeline.stage[2].valid) return;
    if (pipeline.stage[2].opcode == ARM_Opcode::ARM_Suppressed) return; // No-op

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
    if (condition == CONDITION_AL)
    {
        return true;
    }
    return conditionTable[(static_cast<int>(condition) << 4) | (cpuState.cpsr.value >> 28)];
}

void GBA_CPU::AddCycles(uint32_t cycles)
{
    if (cycles == 0) return;

    currentInstructionCycles += cycles;
    currentFrameCycles += cycles;
    totalCycles += cycles;
}

EmulatorCore* GBA_CPU::GetEmulatorCore()
{
    return core;
}

void GBA_CPU::Log(const std::string& message, LogType logType, const char *functionName)
{
    if (core)
    {
       core->Log(message, logType, functionName);
    }
}

u32 GBA_CPU::Read8(u32 address, uint access)
{
    u32 cycles = 0;
    u32 readValue = bus.Read8(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

u32 GBA_CPU::Read16(u32 address, uint access)
{
    u32 cycles = 0;
    u32 readValue = bus.Read16(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

u32 GBA_CPU::Read32(u32 address, uint access)
{
    u32 cycles = 0;
    u32 readValue = bus.Read32(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

void GBA_CPU::Write8(u32 address, u8 value, uint access)
{
    u32 cycles = 0;
    bus.Write8(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

void GBA_CPU::Write16(u32 address, u16 value, uint access)
{
    u32 cycles = 0;
    bus.Write16(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

void GBA_CPU::Write32(u32 address, u32 value, uint access)
{
    u32 cycles = 0;
    bus.Write32(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

u32 GBA_CPU::Read16_Rotated(u32 address, uint access) 
{
    u32 cycles = 0;
    u32 value = bus.Read16(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);

    if (address & 1)
    {
        value = (value >> 8) | (value << 24);
    }

    return value;
}

u32 GBA_CPU::Read32_Rotated(u32 address, uint access) 
{
    u32 cycles = 0;
    u32 value = bus.Read32(address, BusRequester::CPU, &cycles);
    u32 shift = (address & 3) * 8;
    
    AddCycles(cycles);
    return value >> shift | (value << (32 - shift));
}

u32 GBA_CPU::Read8_Signed(u32 address, uint access) 
{ 
    u32 cycles = 0;
    u8 value = bus.Read8(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return SignExtend_8(value); 
}

u32 GBA_CPU::Read16_Signed(u32 address, uint access) 
{ 
    u32 cycles = 0;

    if (address & 1) // Misaligned
    {
        u8 value = bus.Read8(address, BusRequester::CPU, &cycles);
        AddCycles(cycles);
        return SignExtend_8(value);
    }
    else
    {
        u16 value = bus.Read16(address, BusRequester::CPU, &cycles);
        AddCycles(cycles);
        return SignExtend_16(value);
    }
}

void GBA_CPU::InvalidateSequentiality() 
{
    bus.InvalidateSequentiality();
}

void GBA_CPU::WriteRegister(uint index, u32 value)
{ 

    cpuState.registers[index] = value; 
}
