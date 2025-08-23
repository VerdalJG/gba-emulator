#include "Core/GBA_CPU.hpp"


GBA_CPU::GBA_CPU(GBA_Memory& memory) :
    memorySystem(memory)
{

}

GBA_CPU::~GBA_CPU()
{

}

GBA_CPU::InstructionFunction GBA_CPU::dataProcessingTable[GBA_CPU::DATA_PROCESSING_OPCODE_COUNT] = 
{
    &GBA_CPU::LogicalAND,         
    &GBA_CPU::LogicalExclusiveOR,         
    &GBA_CPU::Subtract,        
    &GBA_CPU::ReverseSubtract,         
    &GBA_CPU::Add,       
    &GBA_CPU::AddWithCarry,       
    &GBA_CPU::SubtractWithCarry,         
    &GBA_CPU::ReverseSubtractWithCarry,   
    &GBA_CPU::Test,     
    &GBA_CPU::TestEquivalence,   
    &GBA_CPU::Compare,     
    &GBA_CPU::CompareNegated,    
    &GBA_CPU::LogicalOR,        
    &GBA_CPU::Move,        
    &GBA_CPU::BitClear,      
    &GBA_CPU::MoveNot         
};

void GBA_CPU::Reset()
{

}

void GBA_CPU::Step()
{
    // Fetch
    uint32_t instruction = memorySystem.Read32(ReadProgramCounter(true));
    uint8_t condition = GetConditionBits(instruction);

    // Decode
    InstructionFunction operationToExecute;
    // Thumb mode does not use condition bits
    if (mode == CPUMode::ARM) 
    {
        if (condition == 0xF)
        {
            HandleUndefinedBehavior(instruction); // For now
            return;
        }

        if (InstructionConditionCheck(condition))
        {
            operationToExecute = DecodeARMInstruction(instruction);
        }
    }
    else
    {
        // Handle Thumb mode here
    }
    
    // Execute
    if (!operationToExecute) return; // Condition failed
    (this->*operationToExecute)(instruction); // Dereference due to this being a member function pointer
}

void GBA_CPU::RequestInterrupt()
{
}

uint32_t GBA_CPU::GetValueAtRegister(int registerIndex)
{
    if (registerIndex == 15)
    {
        return ReadProgramCounter(true);
    }
    else
    {
        return registers[registerIndex];
    }
}

uint32_t GBA_CPU::ReadProgramCounter(bool isPartOfInstruction)
{
    // Pipeline offset: if we are reading the PC as part of the behavior of an instruction, 
    // we must simulate the offset that occurs because the actual PC is 2 instructions ahead.
    uint32_t programCounter = registers[15];
    if (isPartOfInstruction)
    {
        // Add to get the actual program counter (PC) - Usually 2 instructions ahead during execution
        programCounter += (mode == CPUMode::Thumb) ? 4 : 8;
    }

    return programCounter;
}


uint8_t GBA_CPU::GetConditionBits(uint32_t instruction)
{
    uint8_t result = (instruction >> CONDITION_SHIFT) & 0xF;
    return result;
}


GBA_CPU::InstructionFunction GBA_CPU::DecodeARMInstruction(uint32_t instruction)
{
    InstructionPattern pattern = static_cast<InstructionPattern>((instruction >> INSTRUCTION_TYPE_SHIFT) & INSTRUCTION_TYPE_MASK);
    switch (pattern)
    {
        case PATTERN_00: // Data processing and misc
        return DecodePattern00(instruction);

        case PATTERN_01: // Single Data Transfer or Undefined (bit 4 decides)
        return DecodePattern01(instruction); 

        case PATTERN_10: // Block Data Transfer (LDM/STM) or Branch
        return DecodePattern10(instruction);

        case PATTERN_11: // Coprocessor or Software Interrupt
        return DecodePattern11(instruction);

        default:
        return &GBA_CPU::HandleUndefinedBehavior;
    }
}

void GBA_CPU::ApplyCPSRFlags(CPSRFlags flags)
{
    uint32_t flagsMask = 0;

    if (flags.N) flagsMask |= (1 << 31);
    if (flags.Z) flagsMask |= (1 << 30);
    if (flags.C) flagsMask |= (1 << 29);
    if (flags.V) flagsMask |= (1 << 28);

    cpsr &= ~(0xF << 28); // Clear bits 31-28 (N,Z,C,V)
    cpsr |= flagsMask; // Apply flags
}


