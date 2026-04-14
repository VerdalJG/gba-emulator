#include "Utils/Integers.hpp"
#include "Core/IO/IO_Addresses.hpp"
#include "Core/Memory/GBA_Memory_Helpers.hpp"

template <typename T>
T GBA_IO::Read(u32 address)
{
    AccessSize access = static_cast<AccessSize>(sizeof(T));

    // Special case: IMC
    if (((address & ~3) & 0xFFFF) == 0x0800) // Is it an IMC mirror?
    {
        address &= IMC + 3; // Handle special case mirroring

        switch (access)
        {
            case AccessSize::Byte: return Read8(address);
            case AccessSize::Halfword: return Read16(address);
            case AccessSize::Word: return Read32(address);
        }
    }
    
    switch (access)
    {
        case AccessSize::Byte: 
        {
            HalfwordPermissions perms = ioPermissions[(address - IO_START) >> 1];
            bool isHighByte = address & 1; // Reading hi or lo byte
            bool allowed = isHighByte ? perms.CanReadHi() : perms.CanReadLo();
            if (allowed)
            {
                return Read8(address);
            }

            return GetOpenBusValue(address);
        }
        
        case AccessSize::Halfword: 
        {
            HalfwordPermissions perms = ioPermissions[(address - IO_START) >> 1];
            bool allowed = perms.CanReadHalf();
            if (allowed)
            {
                return Read16(address);
            }

            return GetOpenBusValue(address);
        }
        
        case AccessSize::Word:
        {
            /*
                Reading from Unused or Write-Only I/O Ports
                Works like above Unused Memory when the entire 32bit memory fragment is 
                Unused (eg. 0E0h) and/or Write-Only (eg. DMA0SAD). And otherwise, 
                returns zero if the lower 16bit fragment is readable 
                (eg. 04Ch=MOSAIC, 04Eh=NOTUSED/ZERO).
            */

            HalfwordPermissions loPerms = ioPermissions[(address - IO_START) >> 1];
            HalfwordPermissions hiPerms = ioPermissions[(address + 2 - IO_START) >> 1];

            bool loAllowed = loPerms.CanReadHalf();
            bool hiAllowed = hiPerms.CanReadHalf();
            
            if (!loAllowed)
            {
                return GetOpenBusValue(address);
            }
            
            if (!hiAllowed)
            {
                return static_cast<T>(Read16(address));
            }

            return Read32(address);
        }
    }
    return 0; // Invalid
}

template <typename T>
void GBA_IO::Write(u32 address, T value)
{
    AccessSize access = static_cast<AccessSize>(sizeof(T));

    // Special case: IMC
    if (((address & ~3) & 0xFFFF) == 0x0800) // Is it an IMC mirror?
    {
        address &= IMC + 3; // Handle special case mirroring

        switch (access)
        {
            case AccessSize::Byte: return Write8(address, value); 
            case AccessSize::Halfword: return Write16(address, value);
            case AccessSize::Word: return Write32(address, value);
        }
    }

    switch (access)
    {
        case AccessSize::Byte: 
        {
            HalfwordPermissions perms = ioPermissions[(address - IO_START) >> 1];
            bool isHighByte = address & 1;
            bool allowed = isHighByte ? perms.CanWriteHi() : perms.CanWriteLo();

            if (allowed)
            {
                Write8(address, value);
            }
            break;
        }

        case AccessSize::Halfword:
        {
            HalfwordPermissions perms = ioPermissions[(address - IO_START) >> 1];
            bool allowed = perms.CanWriteHalf();

            if (allowed)
            {
                Write16(address, value);
            }
            break;
        }

        case AccessSize::Word: return Write32(address, value);
        {
            HalfwordPermissions loPerms = ioPermissions[(address - IO_START) >> 1];
            HalfwordPermissions hiPerms = ioPermissions[(address + 2 - IO_START) >> 1];

            bool loAllowed = loPerms.CanWriteHalf();
            bool hiAllowed = hiPerms.CanWriteHalf();

            if (!loAllowed) return;
            
            if (!hiAllowed)
            {
                return Write16(address, value);
            }

            return Write32(address, value);
        }
    }
}