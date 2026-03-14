#include "Utils/Integers.hpp"

template <typename T>
T GBA_Bus::Read(u32 address, BusRequester requester)
{
    // Align the address always
    u32 alignedAddress = Align<T>(address);

    // Get the region 
    const MemoryRegion* region = memory.GetRegionFromAddress(alignedAddress);

    // Null check and return open bus if unused memory area
    if (!region)
    {
        return OpenBus(alignedAddress);
    }

    // Accessing by 32 bits in a 16 bit-bus width region means 2 accesses
    AccessSize accessSize = std::min(sizeof(T), region->busWidth);
    uint accesses = (sizeof(T) + region->busWidth - 1) / region->busWidth;

    // DMA and CPU are the ones that control cycles
    bool accessCostsCycles = requester == BusRequester::CPU || requester == BusRequester::DMA;

    if (accessCostsCycles) 
    {
        // TODO: Implement prefetch here too
        HandleAccessCycles(alignedAddress, region->type, accessSize, accesses, requester);
    }

    T readResult;

    switch(region->type)
    {
        case RegionType::BIOS: readResult = ReadBIOS<T>(alignedAddress); break;
        case RegionType::EWRAM: readResult = memory.Read<T>(alignedAddress); break;
        case RegionType::IWRAM: readResult = memory.Read<T>(alignedAddress); break;
        case RegionType::IO: readResult = io.Read<T>(alignedAddress); break;
        case RegionType::PaletteRAM: readResult = ReadPaletteRAM<T>(alignedAddress); break;
        case RegionType::VRAM: readResult = ReadVRAM<T>(alignedAddress); break;
        case RegionType::OAM: readResult = ReadOAM<T>(alignedAddress); break;

        // TODO: Implement prefetch
        case RegionType::ROM0:
        case RegionType::ROM1:
        case RegionType::ROM2:
        {
            readResult = memory.Read<T>(alignedAddress);
            break;
        }

        case RegionType::SRAM: readResult = ReadSRAM<T>(address); break;
        default: return OpenBus(alignedAddress);
    }

    UpdateLatestAccessValues(readResult, address, region->type, sizeof(T));
    return readResult;
}

template <typename T>
void GBA_Bus::Write(u32 address, T value, BusRequester requester)
{
    // Align the address always
    u32 alignedAddress = Align<T>(address);

    // Get the region 
    const MemoryRegion* region = memory.GetRegionFromAddress(alignedAddress);

    // Null check and return open bus if unused memory area
    if (!region)
    {
        return;
    }

    // Accessing by 32 bits in a 16 bit-bus width region means 2 accesses
    AccessSize accessSize = std::min(sizeof(T), region->busWidth);
    uint accesses = (sizeof(T) + region->busWidth - 1) / region->busWidth;

    // DMA and CPU are the ones that control cycles
    bool accessCostsCycles = requester == BusRequester::CPU || requester == BusRequester::DMA;

    if (accessCostsCycles) 
    {
        HandleAccessCycles(alignedAddress, region->type, accessSize, accesses, requester);
    }

    switch(region->type)
    {
        case RegionType::EWRAM: memory.Write<T>(alignedAddress); break;
        case RegionType::IWRAM: memory.Write<T>(alignedAddress); break;
        case RegionType::IO: io.Write<T>(alignedAddress); break;
        case RegionType::PaletteRAM: WritePaletteRAM<T>(alignedAddress); break;
        case RegionType::VRAM: WriteVRAM<T>(alignedAddress); break;
        case RegionType::OAM: WriteOAM<T>(alignedAddress); break;

        case RegionType::ROM0:
        case RegionType::ROM1:
        case RegionType::ROM2:
        {
            memory.Write<T>(alignedAddress);
            break;
        }

        case RegionType::SRAM: WriteSRAM<T>(address); break;
        default: return;
    }

    UpdateLatestAccessValues(value, address, region->type, sizeof(T));
}

template <typename T>
T GBA_Bus::ReadBIOS(u32 address)
{
    u32 pc = cpu->ReadRegister(15);
    bool pcWithinBounds = pc <= BIOS_END;
    bool addressWithinBounds = address <= BIOS_END;

    if (!addressWithinBounds)
    {
        return OpenBus(address);
    }

    // Reading from BIOS region:
    // If reading from bios memory the GBA allows to read opcodes or data only if the program counter 
    // is located inside of the BIOS area. If the program counter is not in the BIOS area,
    // reading will return the most recent successfully fetched BIOS opcode
    if (pcWithinBounds)
    {
        u32 wordAligned = address & 3;
        biosLatch = memory.Read<T>(wordAligned);
    }
    else
    {
        core->Log("Illegal BIOS read: 0x{:08X}", address);
    }

    uint shift = (address & 3) * 8;
    u32 readValue = biosLatch >> shift;

    if constexpr (sizeof(T) == Byte)
        return static_cast<u8>(value);

    if constexpr (sizeof(T) == Halfword)
        return static_cast<u16>(value);

    if constexpr (sizeof(T) == Word)
        return biosLatch;

    static_assert(sizeof(T) == Byte || sizeof(T) == Halfword || sizeof(T) == Word);
}

template <typename T>
T GBA_Bus::ReadPaletteRAM(u32 address, BusRequester requester)
{
    if (requester == BusRequester::PPU)
    {
        // TODO: These memory regions can be accessed during H-Blank or V-Blank only 
        // (unless display is disabled by Forced Blank bit in DISPCNT register).
        if (ppu->GetState() == PPUState::ActiveDisplay) 
        {
            return OpenBus();
        }
    }

    return memory.Read<T>(address); 
}

template <typename T>
T GBA_Bus::ReadVRAM(u32 address, BusRequester requester)
{
    if (requester == BusRequester::PPU)
    {
        if (ppu->GetState() == PPUState::ActiveDisplay)
        {
            return OpenBus();
        }
    }

    return memory.Read<T>(address); 
}

template <typename T>
T GBA_Bus::ReadOAM(u32 address, BusRequester requester)
{
    if (requester == BusRequester::PPU)
    {
        // TODO: There is an additional restriction for OAM memory: 
        // Accesses during H-Blank are allowed only if 'H-Blank Interval Free' in DISPCNT is set
        if (ppu->GetState() == PPUState::ActiveDisplay)
        {
            return OpenBus();
        }
    }

    return memory.Read<T>(address); 
}

template <typename T>
T GBA_Bus::ReadSRAM(u32 address)
{
    // TODO: stop prefetch?

    // TODO: Maybe put SRAM/flash in ROM class?
    u32 value = memory.Read<T>(address);

    if constexpr(sizeof(T) == AccessSize::Halfword) value *= 0x0101;
    if constexpr(sizeof(T) == AccessSize::Word) value *= 0x01010101;

    return value;
}