#include <gtest/gtest.h>
#include "Testing/TestCPU.hpp"
#include "Core/CPU_Shifts.hpp"

class ShiftTests : public::testing::Test
{
protected:
    GBA_Memory memory;
    TestCPU cpu{memory};
};

struct ShiftTestCase {
    uint32_t value;
    uint32_t shift;
    bool isImmediate;
    uint32_t expectedValue;
    uint32_t expectedCarry;
    uint32_t cpsr;
};

void RunLSLCase(const ShiftTestCase& testcase, TestCPU& cpu);
void RunCase(const ShiftTestCase& testcase, TestCPU& cpu, std::function<ShifterOperand(uint32_t, uint32_t, bool, GBA_CPU&)> shiftFunc);

TEST_F(ShiftTests, LSLCases)
{
    std::vector<ShiftTestCase> lslCases = 
    {
        {0x00000001,   1, true, 0x00000002, 0, 0},       // simple shift left
        {0x80000000,   1, true, 0x00000000, 1, 0},       // MSB shifts out → carry=1
        {0x12345678,   4, true, 0x23456780, 1, 0},       // mid shift
        {0xFCFA0101,  32, true, 0x00000000, 1, 0},       // shift == 32
        {0xFCFA0101,  33, true, 0x00000000, 0, 0},       // shift > 32
        {0x000000FF,   0, false, 0x000000FF, 1, 0x20000000} // shift==0, reg form, preserves carry-in=1
    };
    
    for (auto& tc : lslCases) 
    {
        RunLSLCase(tc, cpu);
    }

}

TEST_F(ShiftTests, LSRCases)
{
    std::vector<ShiftTestCase> lsrCases = 
    {
        {0x80000000,   1, true, 0x40000000, 0, 0},       // LSR #1
        {0x12345678,   0, true, 0x00000000, 0, 0},       // LSR #32
        {0xFF00FF00,   8, true, 0x00FF00FF, 0, 0},       // LSR #8
        {0xAAAAAAAA,   0, false, 0xAAAAAAAA, 1, 0x20000000}, // reg shift=0 → carry from CPSR
        {0x00000001,   1, false, 0x00000000, 1, 0},       // reg shift=1
        {0xF0000000,  32, false, 0x00000000, 1, 0},       // reg shift=32
        {0xAAAAAAAA,  40, false, 0x00000000, 0, 0}        // reg shift>32
    };

    for (auto& tc : lsrCases) 
    {
        RunCase(tc, cpu, &Op2_LogicalRight);
    }
}

TEST_F(ShiftTests, ASRCases)
{
    std::vector<ShiftTestCase> asrCases = 
    {
        {0x80000000,   1, true, 0xC0000000, 0, 0},       // sign extend negative
        {0x70000000,   4, true, 0x07000000, 0, 0},       // positive value
        {0xF0000000,   4, true, 0xFF000000, 0, 0},       // negative value
        {0x12345678,   0, true, 0x00000000, 0, 0},       // imm shift=0 → ASR #32
        {0x80000000,   0, true, 0xFFFFFFFF, 1, 0},       // imm shift=0 → ASR #32 all ones
        {0xAAAAAAAA,   0, false, 0xAAAAAAAA, 1, 0x20000000}, // reg shift=0 → preserves carry-in
        {0x80000000,  40, false, 0xFFFFFFFF, 1, 0},       // shift > 32 → all sign bits
        {0x70000000,  40, false, 0x00000000, 0, 0}        // shift > 32 → all sign bits
    };

    for (auto& tc : asrCases) 
    {
        RunCase(tc, cpu, &Op2_ArithmeticRight);
    }
}

TEST_F(ShiftTests, RORCases)
{
    std::vector<ShiftTestCase> rorCases = 
    {
        {0x80000001,   1, true, 0xC0000000, 1, 0},       // rotate right 1
        {0x12345678,   4, true, 0x81234567, 1, 0},       // rotate right 4
        {0xAAAAAAAA,   8, true, 0xAAAAAAAA, 1, 0},       // symmetric pattern
        {0xAAAAAAAA,   0, true, 0xD5555555, 0, 0x20000000} // shift=0 → triggers RRX
    };

    for (auto& tc : rorCases) 
    {
        RunCase(tc, cpu, &RotateRight);
    }
}


void RunCase(const ShiftTestCase& testcase, TestCPU& cpu, std::function<ShifterOperand(uint32_t, uint32_t, bool, GBA_CPU&)> shiftFunc)
{
    cpu.SetCPSR(testcase.cpsr);
    ShifterOperand result = shiftFunc(testcase.value, testcase.shift, testcase.isImmediate, cpu);
    EXPECT_EQ(result.value, testcase.expectedValue);
    EXPECT_EQ(result.carryOut, testcase.expectedCarry);
}

void RunLSLCase(const ShiftTestCase& testcase, TestCPU& cpu)
{
    cpu.SetCPSR(testcase.cpsr);
    ShifterOperand result = LogicalLeft(testcase.value, testcase.shift, cpu);
    EXPECT_EQ(result.value, testcase.expectedValue);
    EXPECT_EQ(result.carryOut, testcase.expectedCarry);
}