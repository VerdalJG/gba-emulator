#include <gtest/gtest.h>
#include "InstructionHelpers.hpp"

TEST(InstructionHelperTest, ProcessResultCPSRFlagsShouldSetZeroFlagCorrectly) {
  uint32_t op1 = 5;
  uint32_t op2 = 5;
  uint32_t result = 0;
  CPSRFlags flags = ProcessResultCPSRFlags(result, op1, op2);

  EXPECT_TRUE(flags.Z);
  EXPECT_FALSE(flags.C);
  EXPECT_FALSE(flags.V);
  EXPECT_FALSE(flags.N);

  result = 42;
  flags = ProcessResultCPSRFlags(result, op1, op2);

  EXPECT_FALSE(flags.Z);
  EXPECT_FALSE(flags.C);
  EXPECT_FALSE(flags.V);
  EXPECT_FALSE(flags.N);
}

TEST(InstructionHelperTest, ProcessResultCPSRFlagsShouldSetNegativeFlagCorrectly) {
  uint32_t op1 = -5;
  uint32_t op2 = -2;
  uint32_t result = -1;
  CPSRFlags flags = ProcessResultCPSRFlags(result, op1, op2);

  EXPECT_FALSE(flags.Z);
  EXPECT_FALSE(flags.C);
  EXPECT_FALSE(flags.V);
  EXPECT_TRUE(flags.N);

  result = 42;
  flags = ProcessResultCPSRFlags(result, op1, op2);

  EXPECT_FALSE(flags.Z);
  EXPECT_TRUE(flags.C);
  EXPECT_FALSE(flags.V);
  EXPECT_FALSE(flags.N);
}