#include "Utils/Integers.hpp"
#include "Core/Memory/GBA_Memory.hpp"
#include "Core/IO/GBA_IO.hpp"
#include "Core/CPU/Shifts.hpp"
#include "Utils/BitOperations.hpp"

template <typename T>
T GBA_Bus::Read(u32 address, BusRequester requester)
{
    // Get the region 
    const MemoryRegion* region = memory.GetRegionFromAddress(address);

    u32 originalAddress = address;

    // Null check and return open bus if unused memory area
    if (!region)
    {
        return OpenBus(originalAddress);
    }

    // Align the address always
    if (region->type != RegionType::SRAM)
    {
        address = Align<T>(address);
    }

    // Handle the address mirroring
    address = GetMirroredAddress(address, region);

    // Accessing by 32 bits in a 16 bit-bus width region means 2 accesses
    AccessSize accessSize = static_cast<AccessSize>(std::min(static_cast<unsigned int>(sizeof(T)), static_cast<unsigned int>(region->busWidth)));
    uint accesses = (sizeof(T) + region->busWidth - 1) / region->busWidth;

    // DMA and CPU are the ones that control cycles
    bool accessCostsCycles = requester == BusRequester::CPU || requester == BusRequester::DMA;

    if (accessCostsCycles) 
    {
        // TODO: Implement prefetch here too
        HandleAccessCycles(originalAddress, region, accessSize, accesses, requester);
    }

    T readResult;

    switch(region->type)
    {
        case RegionType::BIOS: readResult = ReadBIOS<T>(address); break;
        case RegionType::EWRAM: readResult = memory.Read<T>(address); break;
        case RegionType::IWRAM: readResult = memory.Read<T>(address); break;
        case RegionType::IO: readResult = io.Read<T>(address); break;
        case RegionType::PaletteRAM: readResult = ReadPaletteRAM<T>(address, requester); break;
        case RegionType::VRAM: readResult = ReadVRAM<T>(address, requester); break;
        case RegionType::OAM: readResult = ReadOAM<T>(address, requester); break;

        // TODO: Implement prefetch
        case RegionType::ROM0:
        case RegionType::ROM1:
        case RegionType::ROM2:
        {
            readResult = memory.Read<T>(address);
            break;
        }

        case RegionType::SRAM: readResult = ReadSRAM<T>(address); break;
        default: return OpenBus(originalAddress);
    }

    UpdateLatestAccessValues(readResult, originalAddress, region->type, static_cast<AccessSize>(sizeof(T)));
    return readResult;
}

template <typename T>
void GBA_Bus::Write(u32 address, T value, BusRequester requester)
{
    // Get the region 
    const MemoryRegion* region = memory.GetRegionFromAddress(address);

    u32 originalAddress = address;

    // Null check and return open bus if unused memory area
    if (!region)
    {
        return;
    }

    // Align the address always
    if (region->type != RegionType::SRAM)
    {
        address = Align<T>(address);
    }

    // Handle the address mirroring
    address = GetMirroredAddress(address, region);

    // Accessing by 32 bits in a 16 bit-bus width region means 2 accesses
    AccessSize accessSize = static_cast<AccessSize>(std::min(static_cast<unsigned int>(sizeof(T)), static_cast<unsigned int>(region->busWidth)));
    uint accesses = (sizeof(T) + region->busWidth - 1) / region->busWidth;

    // DMA and CPU are the ones that control cycles
    bool accessCostsCycles = requester == BusRequester::CPU || requester == BusRequester::DMA;

    if (accessCostsCycles) 
    {
        HandleAccessCycles(originalAddress, region, accessSize, accesses, requester);
    }

    switch(region->type)
    {
        case RegionType::EWRAM: memory.Write<T>(address, value); break;
        case RegionType::IWRAM: memory.Write<T>(address, value); break;
        case RegionType::IO: io.Write<T>(address, value); break;
        case RegionType::PaletteRAM: WritePaletteRAM<T>(address, value); break;
        case RegionType::VRAM: WriteVRAM<T>(address, value); break;
        case RegionType::OAM: WriteOAM<T>(address, value); break;

        case RegionType::ROM0:
        case RegionType::ROM1:
        case RegionType::ROM2:
        {
            memory.Write<T>(address, value);
            break;
        }

        case RegionType::SRAM: WriteSRAM<T>(address, value); break;
        default: return;
    }

    UpdateLatestAccessValues(value, originalAddress, region->type, static_cast<AccessSize>(sizeof(T)));
}

template <typename T>
T GBA_Bus::ReadBIOS(u32 address)
{
    u32 pc = GetPCFromCPU();
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
        u32 wordAligned = address & ~3;
        biosLatch = memory.Read<T>(wordAligned);
    }
    else
    {
        std::string message = "Illegal BIOS read: 0x" + IntToHexString(address);       
        Log(message, LogType::Error);
    }

    uint shift = (address & 3) * 8;
    u32 readValue = biosLatch >> shift;

    if constexpr (sizeof(T) == Byte)
        return static_cast<u8>(readValue);

    if constexpr (sizeof(T) == Halfword)
        return static_cast<u16>(readValue);

    if constexpr (sizeof(T) == Word)
        return biosLatch;

    static_assert(sizeof(T) == Byte || sizeof(T) == Halfword || sizeof(T) == Word);
}

template <typename T>
T GBA_Bus::ReadPaletteRAM(u32 address, BusRequester requester)
{
    if (requester != BusRequester::CPU)
    {
        // Can be accessed during H-Blank or V-Blank only 
        // (unless display is disabled by Forced Blank bit in DISPCNT register).

        if (!ppu->ForcedBlank() && !ppu->InHBlank() && !ppu->InVBlank())
        {
            return OpenBus(address);
        }
    }

    return memory.Read<T>(address); 
}

template <typename T>
T GBA_Bus::ReadVRAM(u32 address, BusRequester requester)
{
    if (requester != BusRequester::CPU)
    {
        // Can be accessed during H-Blank or V-Blank only 
        // (unless display is disabled by Forced Blank bit in DISPCNT register).

        if (!ppu->ForcedBlank() && !ppu->InHBlank() && !ppu->InVBlank())
        {
            return OpenBus(address);
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

        DisplayControl dispcnt = ppu->GetLCDRegisters().dispcnt;
        if (!dispcnt.fields.hBlankIntervalFree || (!dispcnt.fields.forcedBlank && ppu->GetState() == PPUState::ActiveDisplay))
        {
            return OpenBus(address);
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

    /*
        Accessing SRAM Area by 16bit/32bit
        Reading retrieves 8bit value from specified address, multiplied by 0101h (LDRH) or by 01010101h 
        (LDR). Writing changes the 8bit value at the specified address only, being set to LSB of 
        (source_data ROR (address*8)).
    */

    if constexpr (sizeof(T) == AccessSize::Halfword) value *= 0x0101;
    if constexpr (sizeof(T) == AccessSize::Word) value *= 0x01010101;

    return value;
}

template <typename T> 
void GBA_Bus::WritePaletteRAM(u32 address, T value) 
{
    if constexpr (sizeof(T) == AccessSize::Byte)
    {
        /*
            Writing 8bit Data to Video Memory:
            Writes to Palette (5000000h-50003FFh) writes the new 8bit value to BOTH upper and lower 8bits 
            of the addressed halfword, ie. "[addr AND NOT 1]=data*101h".
        */
        memory.Write<u16>(address, value * 0x101);
    }
    else
    {
        memory.Write<T>(address, value);
    }
}

template <typename T> 
void GBA_Bus::WriteVRAM(u32 address, T value) 
{
    if constexpr (sizeof(T) == AccessSize::Byte)
    {
        /*
            Writing 8bit Data to Video Memory:
            Writes to OBJ (6010000h-6017FFFh) (or 6014000h-6017FFFh in Bitmap mode) are ignored, 
            the memory content remains unchanged.

            Writes to BG (6000000h-600FFFFh) (or 6000000h-6013FFFh in Bitmap mode) writes the new 8bit 
            value to BOTH upper and lower 8bits of the addressed halfword, ie. "[addr AND NOT 1]=data*101h".
        */

        if (ppu->IsWithinVRAM_OBJBoundary(address & ~1))
        {
            return;
        }
        else
        {
            memory.Write<u16>(address & ~1, value * 0x101);
        }
    }
    else
    {
        memory.Write<T>(address, value);
    }
}

template <typename T> 
void GBA_Bus::WriteOAM(u32 address, T value) 
{
    /*
        Writing 8bit Data to Video Memory:
        Writes to OAM (7000000h-70003FFh) are ignored, the memory content remains unchanged.
    */

    if constexpr (sizeof(T) == AccessSize::Byte)
    {
        return;
    }
    else
    {
        memory.Write<T>(address, value);
    }
}

template <typename T> 
void GBA_Bus::WriteSRAM(u32 address, T value) 
{
    /*
        Writing changes the 8bit value at the specified address only, 
        being set to LSB of (source_data ROR (address*8)).
    */

    uint shift = (address & (sizeof(T) - 1)) * 8;
    u8 byte = static_cast<u8>((value >> shift) & 0xFF);

    memory.Write<u8>(address, byte);
}