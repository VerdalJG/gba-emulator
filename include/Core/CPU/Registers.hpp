#pragma once
#include "Utils/Integers.hpp"
#include <array>

enum class Mode
{
    USR = 0x10, // User
    FIQ = 0x11, // Fast Interrupt
    IRQ = 0x12, // Interrupt
    SVC = 0x13, // Supervisor
    ABT = 0x17, // Abort
    UND = 0x1B, // Undefined
    SYS = 0x1F  // System 
};

enum ExceptionBank
{
    BANK_FIQ  = 0,
    BANK_SVC  = 1,
    BANK_ABT  = 2,
    BANK_IRQ  = 3,
    BANK_UND  = 4,
    BANK_COUNT,
};

enum FIQ_BankedRegister
{
    FIQ_R8  = 0,
    FIQ_R9  = 1,
    FIQ_R10 = 2,
    FIQ_R11 = 3,
    FIQ_R12 = 4,
    FIQ_RCOUNT
};

enum BankedRegister
{
    BANK_R13 = 0,
    BANK_R14 = 1,
    BANK_RCOUNT
};

union StatusRegister
{
    StatusRegister() {}
    StatusRegister(uint32_t val) { value = val; }

    struct 
    {
        Mode mode : 5;
        uint thumb : 1;
        uint fiq_disable : 1;
        uint irq_disable : 1;
        uint reserved : 19;
        uint q : 1; // Sticky overflow
        uint v : 1; // Signed overflow
        uint c : 1; // Carry / borrow
        uint z : 1; // Zero
        uint n : 1; // Negative
    } fields;
    u32 value;
};

struct CPU_Registers
{
    // General purpose registers
    union 
    {
        struct 
        {
            u32 r0;
            u32 r1;
            u32 r2;
            u32 r3;
            u32 r4;
            u32 r5;
            u32 r6;
            u32 r7;
            u32 r8;
            u32 r9;
            u32 r10;
            u32 r11;
            u32 r12;
            u32 r13;
            u32 r14;
            u32 r15;
        };
        std::array<u32, 16> registers;
    };

    // Banked Registers
    std::array<u32, FIQ_RCOUNT> fiqR8_R12;
    std::array<std::array<u32, BANK_RCOUNT>, BANK_COUNT> bankedR13_R14s;

    // Program Status Registers
    StatusRegister cpsr;
    StatusRegister spsr[BANK_COUNT];

    CPU_Registers() { Reset(); }

    void Reset()
    {
        // Clear registers
        for (int i = 0; i < registers.size(); i++)
        {
            registers[i] = 0;
        }

        for (int i = 0; i < FIQ_RCOUNT; i++)
        {
            fiqR8_R12[i] = 0;
        }

        for (int i = 0; i < BANK_COUNT; i++)
        {
            for (int j = 0; j < BANK_RCOUNT; j++)
            {
                bankedR13_R14s[i][j] = 0;
            }
        }

        StatusRegister new_cpsr;
        new_cpsr.fields.mode = Mode::SVC;
        new_cpsr.fields.irq_disable = 1;
        new_cpsr.fields.fiq_disable = 1;

        cpsr = new_cpsr;
    }
};
