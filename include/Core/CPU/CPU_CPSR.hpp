#include <cstdint>

class GBA_CPU;

inline uint32_t CPSR_IsNegative(uint32_t result)
{
    return result & 0x80000000;
}

inline uint32_t CPSR_IsZero(uint32_t result)
{
    return (result == 0) ? (1 << 30) : 0; 
}

inline uint32_t CPSR_CarryFrom(uint64_t op1, uint64_t op2, uint64_t carryIn)
{
    uint64_t fullResult = op1 + op2 + carryIn;
    return (fullResult >> 32) << 29;
}

inline uint32_t CPSR_NOTCarryFrom(uint64_t op1, uint64_t op2, uint64_t carryIn)
{
    // C = 1 if no borrow (op1 >= op2 + carryIn)
    uint64_t fullBorrow = (static_cast<uint64_t>(op2) + (carryIn ? 0 : 1));
    return op1 >= fullBorrow ? (1 << 29) : 0;
}

uint32_t CPSR_OverflowFrom(uint32_t op1, uint32_t op2, uint32_t result, bool isSub);

void HandleProgramCounterCPSRCase(GBA_CPU& cpu);
