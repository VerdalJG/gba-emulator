#include <gtest/gtest.h>
#include "Testing/TestCPU.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/CPU/Instructions/Shifts.hpp"
#include "Core/CPU/Instructions/AddressingMode1.hpp"
#include "Utils/Logger.hpp"
#include <array>

class ShiftTests : public::testing::Test
{
protected:
    EmulatorCore core{nullptr};
    TestCPU cpu{&core};
};

struct ShiftTestCase 
{
    uint16_t shifterOperandBits;
    uint32_t expectedValue;
    uint32_t expectedCarry;
    uint32_t cpsr;

    // For register shifts
    uint32_t valueToPutInR0;
};

void RunCase(const ShiftTestCase& testcase, TestCPU& cpu, int testNum);

TEST_F(ShiftTests, LSLCases)
{
    std::vector<ShiftTestCase> lslCases = 
    {
        // LSL #1 
        {(1 << 7) | (0 << 5) | (0 << 4) | 0, 0x00000002u, 0, 0, 0x00000001u},

        // LSL #1 checking carry functionality
        {(1 << 7) | (0 << 5) | (0 << 4) | 0, 0x00000000u, 1, 0, 0x80000000u},

        // LSL #4
        {(4 << 7) | (0 << 5) | (0 << 4) | 0, 0x23456780u, 1, 0, 0x12345678u},

        // R0 LSL R0
        {(0 << 7) | (0 << 5) | (1 << 4) | 0, 0, 0, 0x20000000u, 0x000000FFu}
    };
    
    for (int i = 0; i < lslCases.size(); i++) 
    {
        RunCase(lslCases[i], cpu, i);
    }

}

TEST_F(ShiftTests, LSRCases)
{
    std::vector<ShiftTestCase> lsrCases = 
    {
        // LSR #1 immediate
        {(1 << 7) | (1 << 5) | (0 << 4) | 0, 0x40000000u, 0, 0, 0x80000000u},

        // LSR #32 immediate
        {(32 << 7) | (1 << 5) | (0 << 4) | 0, 0x00000000u, 0, 0, 0x12345678u},

        // LSR #8 immediate
        {(8 << 7) | (1 << 5) | (0 << 4) | 0, 0x00FF00FFu, 0, 0, 0xFF00FF00u},

        // shift=0, register form → carry from CPSR, RsIndex = 0 (R0 contains 0)
        {(0 << 7) | (1 << 5) | (1 << 4) | 0, 0x00000000u, 1, 0x20000000u, 0x00000000u},

        // register shift=1, RsIndex = 0, R0 = 1
        {(0 << 7) | (1 << 5) | (1 << 4) | 0, 0x00000001u, 1, 0, 0x00000003u},

        // register shift=32, RsIndex = 0, R0 = 32
        {(0 << 7) | (1 << 5) | (1 << 4) | 0, 0x00000000u, 0, 0, 0x00000020u},

        // register shift > 32, RsIndex = 0, R0 = 40
        {(0 << 7) | (1 << 5) | (1 << 4) | 0, 0x00000000u, 0, 0, 0x00000028u}
    };

    for (int i = 0; i < lsrCases.size(); i++) 
    {
        RunCase(lsrCases[i], cpu, i);
    }
}

TEST_F(ShiftTests, ASRCases)
{
    std::vector<ShiftTestCase> asrCases = 
    {
        // sign extend negative, immediate shift=1
        {(1 << 7) | (2 << 5) | (0 << 4) | 0, 0xC0000000u, 0, 0, 0x80000000u},

        // positive value, immediate shift=4
        {(4 << 7) | (2 << 5) | (0 << 4) | 0, 0x07000000u, 0, 0, 0x70000000u},

        // negative value, immediate shift=4
        {(4 << 7) | (2 << 5) | (0 << 4) | 0, 0xFF000000u, 0, 0, 0xF0000000u},

        // imm shift=0 → ASR #32
        {(0 << 7) | (2 << 5) | (0 << 4) | 0, 0x00000000u, 0, 0, 0x12345678u},

        // imm shift=0 → ASR #32 all ones
        {(0 << 7) | (2 << 5) | (0 << 4) | 0, 0xFFFFFFFFu, 1, 0, 0x80000000u},

        // reg shift=0 → preserves carry-in
        {(0 << 7) | (2 << 5) | (1 << 4) | 0, 0xFFFFFFFFu, 1, 0x20000000u, 0xAAAAAAAAu},

        // shift > 32 → all sign bits
        {(0 << 7) | (2 << 5) | (1 << 4) | 0, 0xFFFFFFFFu, 1, 0, 0x80000000u},

        // shift > 32 → all sign bits, positive
        {(0 << 7) | (2 << 5) | (1 << 4) | 0, 0x00000000u, 0, 0, 0x70000000u}
    };

    for (int i = 0; i < asrCases.size(); i++) 
    {
        RunCase(asrCases[i], cpu, i);
    }
}

TEST_F(ShiftTests, RORCases)
{
    std::vector<ShiftTestCase> rorCases = 
    {
        // rotate right 1 immediate
        {(1 << 7) | (3 << 5) | (0 << 4) | 0, 0xC0000000u, 1, 0, 0x80000001},

        // rotate right 4 immediate
        {(4 << 7) | (3 << 5) | (0 << 4) | 0, 0x81234567u, 1, 0, 0x12345678u},

        // rotate right 8 immediate, symmetric
        {(8 << 7) | (3 << 5) | (0 << 4) | 0, 0xAAAAAAAAu, 1, 0, 0xAAAAAAAAu},

        // shift=0 → triggers RRX, register form
        {(0 << 7) | (3 << 5) | (1 << 4) | 0, 0xD5555555u, 0, 0x20000000u, 0xAAAAAAAAu}
    };

    for (int i = 0; i < rorCases.size(); i++) 
    {
        RunCase(rorCases[i], cpu, i);
    }
}


void RunCase(const ShiftTestCase& testcase, TestCPU& cpu, int testNum)
{
    cpu.SetCPSR(testcase.cpsr);
    cpu.SetValueAtRegister(0, testcase.valueToPutInR0);
    ShifterOperand result = CalculateOp2_AddressingMode1(testcase.shifterOperandBits, false, cpu);
    printf("Test Number: %i\n", testNum + 1);
    EXPECT_EQ(result.value, testcase.expectedValue);
    EXPECT_EQ(result.carryOut, testcase.expectedCarry);
}