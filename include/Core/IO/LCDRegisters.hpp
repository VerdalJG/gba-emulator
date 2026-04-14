#pragma once

#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Utils/Integers.hpp"

class GBA_PPU;

struct DisplayControl
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }

    void Reset() { value = 0; }
    void ResetToPostBIOSValue() { value = 0x80; }

    const u16 writeMask = 0xFFF7;

    union
    {
        struct 
        {
            u16 bgMode : 3; // 6 & 7 = prohibited
            u16 cgbMode : 1; // Can only be set by BIOS opcodes
            u16 frameSelect : 1; // For BG modes 4, 5 only
            u16 hBlankIntervalFree : 1; // 1 = Allow access to OAM during HBlank
            u16 objCharacterVramMapping : 1; // 0 = two dimensional, 1 = one dimensional
            u16 forcedBlank : 1; // 1 = Allow fast access to VRAM, Palette, OAM
            u16 screenDisplayBG : 4; // BG 0-3, 0 = off, 1 = on
            u16 screenDisplayOBJ : 1; 
            u16 windowDisplayFlag : 2;
            u16 objWindowDisplayFlag : 1;
        } fields;
        u16 value;
    };
};

struct GreenSwap
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); } 

    void Reset() { value = 0; }
    
    const u16 writeMask = 0x1;

    union
    {
        struct 
        {
            u16 greenSwap : 1; // (0 = Normal, 1 = Swap)
            u16 unused : 15;
        } fields;
        u16 value;
    };
};

struct DisplayStatus
{
    u8 Read8(int byteToRead);

    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    
    void Reset() { value = 0; }

    const u16 writeMask = 0xFF38;

    union
    {
        struct 
        {
            u16 vBlank : 1; // (Read only) (1=VBlank) (set in line 160..226; not 227)
            u16 hBlank : 1; // (Read only) (1=HBlank) (toggled in all lines, 0..227)
            u16 vCounter : 1; // (Read only) (1=Match)  (set in selected line)
            u16 vBlankIRQ : 1; // (1=Enable) (R/W)
            u16 hBlankIRQ : 1; // (1=Enable) (R/W)
            u16 vCounterIRQ : 1; // (1=Enable) (R/W)
            u16 unused : 2; // has NDS behavior on these 2 bits, unused in GBA
            u16 vCountSetting : 8; // (LYC) (0..227) (R/W)
        } fields;
        u16 value;

        /*
            Notes:
            The V-Count-Setting value is much the same as LYC of older gameboys, 
            when its value is identical to the content of the VCOUNT register then the
            V-Counter flag is set (Bit 2), and (if enabled in Bit 5) an interrupt is requested.

            Although the drawing time is only 960 cycles (240*4), the H-Blank flag is "0" for a total of 1006 cycles.
        */
    };

    
    GBA_PPU* ppu = nullptr; // TODO: NULL, NEED TO SET
};

struct VerticalCounter // Read-only
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }

    void Reset() { value = 0; }

    union
    {
        struct 
        {
            u16 currentScanline : 8; // (LY) (0-227)
            u16 unused : 8; // Hardwired to 0
        } fields;
        u16 value;

        /*
            Indicates the currently drawn scanline, values in range from 160..227 indicate 'hidden' scanlines within VBlank
            Note: This is very similar to the 'LY' register of older gameboys.
        */
    };
};

struct BackgroundControl // R/W
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }

    void Write8(int byteToWrite, u8 value) 
    {
        u16 mask = id < 2 ? writeMaskBG0_BG1 : writeMaskBG2_BG3;
        IO::Write8Masked(this->value, byteToWrite, value, mask); 
    }

    void Reset() { value = 0; }

    BackgroundControl() = default;
    BackgroundControl(int id) : id(id) {};

    const u16 writeMaskBG0_BG1 = 0xDFCF;
    const u16 writeMaskBG2_BG3 = 0xFFCF;
    const int id = -1;

    union
    {
        struct
        {
            u16 bgPriority : 2; // (0-3, 0=Highest) If equal priority then BG0 is the highest, and BG3 the lowest priority.
            u16 characterBaseBlock : 2; // (0-3, in units of 16 KBytes) (=BG Tile Data)
            u16 unused : 2; // (must be zero) (except in NDS mode: MSBs of char base)
            u16 mosaic : 1; // (0=Disable, 1=Enable)
            u16 colors : 1; // (0=16/16, 1=256/1)
            u16 screenBaseBlock : 5; // (0-31, in units of 2 KBytes) (=BG Map Data)
            u16 displayAreaOverflow : 1; // Not used in BG0/BG1 (unless NDS), for BG2/BG3: (0=Transparent, 1=Wraparound)
            u16 screenSize : 2; // (0-3)
        } fields;
        u16 value;

            /*
        Internal Screen Size (dots) and size of BG Map (bytes):
        Value  Text Mode      Rotation/Scaling Mode
        0      256x256 (2K)   128x128   (256 bytes)
        1      512x256 (4K)   256x256   (1K)
        2      256x512 (4K)   512x512   (4K)
        3      512x512 (8K)   1024x1024 (16K)
        
        In 'Text Modes', the screen size is organized as follows: 
        The screen consists of one or more 256x256 pixel (32x32 tiles) areas. 
        When Size=0: only 1 area (SC0), 
        when Size=1 or Size=2: two areas (SC0,SC1 either horizontally or vertically arranged next to each other), 
        when Size=3: four areas (SC0,SC1 in upper row, SC2,SC3 in lower row). 
        Whereas SC0 is defined by the normal BG Map base address (Bit 8-12 of BGxCNT), 
        SC1 uses same address +2K, SC2 address +4K, SC3 address +6K. When the screen is scrolled it'll always wraparound.

        In 'Rotation/Scaling Modes', the screen size is organized as follows, 
        only one area (SC0) of variable size 128x128..1024x1024 pixels (16x16..128x128 tiles) exists. 
        When the screen is rotated/scaled (or scrolled?) so that the LCD viewport reaches outside of the background/screen area, 
        then BG may be either displayed as transparent or wraparound (Bit 13 of BGxCNT).
    */
    };
};

struct BackgroundOffset // Write only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }

    void Reset() { value = 0; }

    const u16 writeMask = 0x01FF;

    union
    {
        struct 
        {
            u16 offset : 9; // (0-511)
            u16 unused : 7;
        } fields;
        u16 value;

        /*
            Specifies the coordinate of the upperleft first visible dot of BG0 background layer, 
            ie. used to scroll the BG0 area.

            The above BG scrolling registers are exclusively used in Text modes, ie. for all layers in BG Mode 0, 
            and for the first two layers in BG mode 1.
            In other BG modes (Rotation/Scaling and Bitmap modes) above registers are ignored. 
            Instead, the screen may be scrolled by modifying the BG Rotation/Scaling Reference Point registers.
        */
    };
};

struct BackgroundRefPointCoords // Write-only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }

    const u32 writeMask = 0x0FFFFFFF;

    union
    {
        struct 
        {
            u32 fraction : 8;
            u32 integer : 19;
            u32 sign : 1;
            u32 unused : 4;
        } fields;
        u32 value;

        /*
            Specifies the coordinate of the upperleft first visible dot of BG0 background layer, 
            ie. used to scroll the BG0 area.

            The above BG scrolling registers are exclusively used in Text modes, ie. for all layers in BG Mode 0, 
            and for the first two layers in BG mode 1.
            In other BG modes (Rotation/Scaling and Bitmap modes) above registers are ignored. 
            Instead, the screen may be scrolled by modifying the BG Rotation/Scaling Reference Point registers.
        */
    };
};

struct BackgroundScalingParameter // Write-only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value); }
    void Reset() { value = 0; }

    void ResetToPostBIOSValue() { value = (id == 0 || id == 3) ? 0x100 : 0; }
    const int id = -1;

    union
    {
        struct 
        {
            u16 fraction : 8;
            u16 integer : 7;
            u16 sign : 1;
        } fields;
        u16 value;

        /*
            dx (PA) and dy (PC)
            When transforming a horizontal line, dx and dy specify the resulting gradient and magnification for that line. 
            For example: 
            Horizontal line, length=100, dx=1, and dy=1. 
            The resulting line would be drawn at 45 degrees, f(y)=1/1*x. 
            Note that this would involve that line is magnified, the new length is SQR(100^2+100^2)=141.42. 
            That's the old a^2 + b^2 = c^2 formula.

            dmx (PB) and dmy (PD)
            These values define the resulting gradient and magnification for transformation of vertical lines. 
            However, when rotating a square area (which is surrounded by horizontal and vertical lines), 
            then the desired result should be usually a rotated <square> area (ie. not a parallelogram, for example).
            Thus, dmx and dmy must be defined in direct relationship to dx and dy, taking the example above, 
            we'd have to set dmx=-1, and dmy=1, f(x)=-1/1*y.

            Area Overflow
            In result of rotation/scaling it may often happen that areas outside of the actual BG area
            become moved into the LCD viewport. Depending of the Area Overflow bit (BG2CNT and BG3CNT, Bit 13) 
            these areas may be either displayed (by wrapping the BG area), or may be displayed transparent.

            This works only in BG modes 1 and 2. The area overflow is ignored in Bitmap modes (BG modes 3-5), 
            the outside of the Bitmaps is always transparent.

            --- more details and confusing or helpful formulas ---

            The following parameters are required for Rotation/Scaling
            Rotation Center X and Y Coordinates (x0,y0)
            Rotation Angle                      (alpha)
            Magnification X and Y Values        (xMag,yMag)
            The display is rotated by 'alpha' degrees around the center.
            The displayed picture is magnified by 'xMag' along x-Axis (Y=y0) and 'yMag' along y-Axis (X=x0).

            Calculating Rotation/Scaling Parameters A-D
            A = Cos (alpha) / xMag    ;distance moved in direction x, same line
            B = Sin (alpha) / xMag    ;distance moved in direction x, next line
            C = Sin (alpha) / yMag    ;distance moved in direction y, same line
            D = Cos (alpha) / yMag    ;distance moved in direction y, next line

            Calculating the position of a rotated/scaled dot
            Using the following expressions,
            x0,y0    Rotation Center
            x1,y1    Old Position of a pixel (before rotation/scaling)
            x2,y2    New position of above pixel (after rotation scaling)
            A,B,C,D  BG2PA-BG2PD Parameters (as calculated above)
            the following formula can be used to calculate x2,y2:
            x2 = A(x1-x0) + B(y1-y0) + x0
            y2 = C(x1-x0) + D(y1-y0) + y0

        */
    };
};

struct WindowHorizontalDimensions // Write-only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value); }
    void Reset() { value = 0; }

    union
    {
        struct 
        {
            u16 x2 : 8; // Rightmost coordinate of window, plus 1
            u16 x1 : 8; // Leftmost coordinate of window
        } fields;
        u16 value;

        // Garbage values of X2>240 or X1>X2 are interpreted as X2=240.
    };
};

struct WindowVerticalDimensions // Write-only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value); }
    void Reset() { value = 0; }

    union
    {
        struct 
        {
            u16 y2 : 8; // Bottom-most coordinate of window, plus 1
            u16 y1 : 8; // Top-most coordinate of window
        } fields;
        u16 value;

        // Garbage values of Y2>160 or Y1>Y2 are interpreted as Y2=160.
    };
};

struct WindowControl_In // Read/Write
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x3F3F;
    
    union
    {
        struct 
        {
            u16 bgEnable_0 : 4; // Window 0 BG0-BG3 Enable Bits  (0=No Display, 1=Display)
            u16 objEnable_0 : 1; // Window 0 OBJ Enable Bit      (0=No Display, 1=Display)
            u16 colorSFX_0 : 1; // Window 0 Color Special Effect (0=Disable, 1=Enable)
            u16 unused : 2;
            u16 bgEnable_1 : 4; // Window 0 BG0-BG3 Enable Bits  (0=No Display, 1=Display)
            u16 objEnable_1 : 1; // Window 0 OBJ Enable Bit      (0=No Display, 1=Display)
            u16 colorSFX_1 : 1; // Window 0 Color Special Effect (0=Disable, 1=Enable)
            u16 unused2 : 2;
        } fields;
        u16 value;

        /*
            The OBJ Window
            The dimension of the OBJ Window is specified by OBJs which are having the "OBJ Mode" 
            attribute being set to "OBJ Window". Any non-transparent dots of any such OBJs are 
            marked as OBJ Window area. The OBJ itself is not displayed.

            The color, palette, and display priority of these OBJs are ignored. Both DISPCNT Bits 
            12 and 15 must be set when defining OBJ Window region(s).

            Window Priority
            In case that more than one window is enabled, and that these windows do overlap, 
            Window 0 has the highest priority, Window 1 medium, and Obj Window lowest priority. 
            Outside of Window has zero priority, it is used for all dots which are not inside of any window region.
        */ 
    };
};

struct WindowControl_Out // Read/Write
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x3F3F;
    
    union
    {
        struct 
        {
            u16 bgEnable_Out : 4; // Window 0 BG0-BG3 Enable Bits  (0=No Display, 1=Display)
            u16 objEnable_Out : 1; // Window 0 OBJ Enable Bit      (0=No Display, 1=Display)
            u16 colorSFX_Out : 1; // Window 0 Color Special Effect (0=Disable, 1=Enable)
            u16 unused : 2;
            u16 bgEnable_Obj : 4; // Window 0 BG0-BG3 Enable Bits  (0=No Display, 1=Display)
            u16 objEnable_Obj : 1; // Window 0 OBJ Enable Bit      (0=No Display, 1=Display)
            u16 colorSFX_Obj : 1; // Window 0 Color Special Effect (0=Disable, 1=Enable)
            u16 unused2 : 2;
        } fields;
        u16 value;

        /*
            The OBJ Window
            The dimension of the OBJ Window is specified by OBJs which are having the "OBJ Mode" 
            attribute being set to "OBJ Window". Any non-transparent dots of any such OBJs are 
            marked as OBJ Window area. The OBJ itself is not displayed.

            The color, palette, and display priority of these OBJs are ignored. Both DISPCNT Bits 
            12 and 15 must be set when defining OBJ Window region(s).

            Window Priority
            In case that more than one window is enabled, and that these windows do overlap, 
            Window 0 has the highest priority, Window 1 medium, and Obj Window lowest priority. 
            Outside of Window has zero priority, it is used for all dots which are not inside of any window region.
        */ 
    };
};

struct Mosaic // Write-only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value); }
    void Reset() { value = 0; }

    union
    {
        struct 
        {
            u16 bgHSize : 4; // (minus 1)
            u16 bgVSize : 4; // (minus 1)
            u16 objHSize : 4; // (minus 1)
            u16 objVSize : 4; // (minus 1)
        } fields;
        u16 value;

        /*
            The Mosaic function can be separately enabled/disabled for BG0-BG3 by BG0CNT-BG3CNT Registers, 
            as well as for each OBJ0-127 by OBJ attributes in OAM memory. 
            Also, setting all of the bits below to zero effectively disables the mosaic function.

            Example: When setting H-Size to 5, then pixels 0-5 of each display row are colorized as pixel 0, 
            pixels 6-11 as pixel 6, pixels 12-17 as pixel 12, and so on.

            Normally, a 'mosaic-pixel' is colorized by the color of the upperleft covered pixel. 
            In many cases it might be more desireful to use the color of the pixel in the center of the covered area - 
            this effect may be gained by scrolling the background (or by adjusting the OBJ position, 
            as far as upper/left rows/columns of OBJ are transparent).
        
        */
    };
};

struct BlendControl // Read/Write
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x3FFF;

    union
    {
        struct 
        {
            u16 bg0Target1 : 1; // BG0 1st Target Pixel (Background 0)
            u16 bg1Target1 : 1; // BG1 1st Target Pixel (Background 1)
            u16 bg2Target1 : 1; // BG2 1st Target Pixel (Background 2)
            u16 bg3Target1 : 1; // BG3 1st Target Pixel (Background 3)
            u16 objTarget1 : 1; // OBJ 1st Target Pixel (Top-most OBJ pixel)
            u16 backdropTarget1 : 1; // BD 1st Target Pixel (Backdrop)
            u16 colorSFX : 2; // Color Special Effect 
            // 0 = None                (Special effects disabled)
            // 1 = Alpha Blending      (1st+2nd Target mixed)
            // 2 = Brightness Increase (1st Target becomes whiter)
            // 3 = Brightness Decrease (1st Target becomes blacker)
            u16 bg0Target2 : 1; // BG0 2nd Target Pixel (Background 0)
            u16 bg1Target2 : 1; // BG1 2nd Target Pixel (Background 1)
            u16 bg2Target2 : 1; // BG2 2nd Target Pixel (Background 2)
            u16 bg3Target2 : 1; // BG3 2nd Target Pixel (Background 3)
            u16 objTarget2 : 1; // OBJ 2nd Target Pixel (Top-most OBJ pixel)
            u16 backdropTarget2 : 1; // BD 2nd Target Pixel (Backdrop)
        } fields;
        u16 value;

        /*
            Selects the 1st Target layer(s) for special effects. For Alpha Blending/Semi-Transparency, 
            it does also select the 2nd Target layer(s), which should have next lower display priority as the 1st Target.
            However, any combinations are possible, including that all layers may be selected as both 1st+2nd target, 
            in that case the top-most pixel will be used as 1st target, and the next lower pixel as 2nd target.
        */
    };
};

struct BlendAlpha // (Read/Write) - Used for Color Special Effects Mode 1, and for Semi-Transparent OBJs.
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x1F1F;

    union 
    {
        struct 
        {
            u16 evaCoefficient : 5; // EVA Coefficient (1st Target) (0..16 = 0/16..16/16, 17..31=16/16)
            u16 unused : 3;
            u16 evbCoefficient : 5; // EVB Coefficient (2nd Target) (0..16 = 0/16..16/16, 17..31=16/16)
            u16 unused2 : 3;
        } fields;
        u16 value;

        /*
            For this effect, the top-most non-transparent pixel must be selected as 1st Target, 
            and the next-lower non-transparent pixel must be selected as 2nd Target, if so - and only if so, 
            then color intensities of 1st and 2nd Target are mixed together by using the 
            parameters in BLDALPHA register, for each pixel each R, G, B intensities are calculated separately:

            I = MIN ( 31, I1st*EVA + I2nd*EVB )
            Otherwise - for example, if only one target exists, or if a non-transparent non-2nd-target pixel is moved 
            between the two targets, or if 2nd target has higher display priority than 1st target - 
            then only the top-most pixel is displayed (at normal intensity, regardless of BLDALPHA).
        */
    };
    
};

struct BlendY // (Write-only) - Used for Color Special Effects Modes 2 and 3.
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x1F1F;

    union 
    {
        struct 
        {
            u16 evyCoefficient : 5; // EVY Coefficient (Brightness) (0..16 = 0/16..16/16, 17..31=16/16)
            u16 unused : 11;
        } fields;
        u16 value;

        /*
            For each pixel each R, G, B intensities are calculated separately:
            I = I1st + (31-I1st)*EVY   ;For Brightness Increase
            I = I1st - (I1st)*EVY      ;For Brightness Decrease
            The color intensities of any selected 1st target surface(s) are increased or decreased by using 
            the parameter in BLDY register.

            Semi-Transparent OBJs
            OBJs that are defined as 'Semi-Transparent' in OAM memory are always selected as 1st Target 
            (regardless of BLDCNT Bit 4), and are always using Alpha Blending mode (regardless of BLDCNT Bit 6-7).
            The BLDCNT register may be used to perform Brightness effects on the OBJ (and/or other BG/BD layers). 
            However, if a semi-transparent OBJ pixel does overlap a 2nd target pixel, then semi-transparency 
            becomes priority, and the brightness effect will not take place (neither on 1st, nor 2nd target).

            The OBJ Layer
            Before special effects are applied, the display controller computes the OBJ priority ordering, and 
            isolates the top-most OBJ pixel. In result, only the top-most OBJ pixel is recursed at the time when 
            processing special effects. Ie. alpha blending and semi-transparency can be used for OBJ-to-BG or 
            BG-to-OBJ , but not for OBJ-to-OBJ.
        */
    };
    
};


