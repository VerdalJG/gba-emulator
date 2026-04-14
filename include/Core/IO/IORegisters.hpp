#pragma once

#include "Core/IO/LCDRegisters.hpp"

#include <array>

struct IO_LCDRegisters
{
    DisplayControl dispcnt;
    GreenSwap greenswap;
    DisplayStatus dispstat;
    VerticalCounter vcount;
    std::array<BackgroundControl, 4> bgcnt = { 0, 1, 2, 3 }; // ID's for each bgcnt

    std::array<BackgroundOffset, 4> bghofs; // Background X-offsets
    std::array<BackgroundOffset, 4> bgvofs; // Background Y-offsets

    std::array<BackgroundScalingParameter, 4> bg2Params = { 0, 1, 2, 3 }; // BG2 Scaling params A-D
    BackgroundRefPointCoords bg2XCoord; // BG2 Reference point Coordinates X (low 16 bits and high 12 bits)
    BackgroundRefPointCoords bg2YCoord; // BG2 Reference point Coordinates Y (low 16 bits and high 12 bits)s

    std::array<BackgroundScalingParameter, 4> bg3Params = { 0, 1, 2, 3 }; // BG3 Scaling params A-D
    BackgroundRefPointCoords bg3XCoord; // BG3 Reference point Coordinates X (low 16 bits and high 12 bits)
    BackgroundRefPointCoords bg3YCoord; // BG3 Reference point Coordinates Y (low 16 bits and high 12 bits)

    std::array<WindowHorizontalDimensions, 2> winH; // Window 0 and 1 horizontal dimensions (W)
    std::array<WindowVerticalDimensions, 2> winV; // Window 0 and 1 vertical dimensions (W)

    WindowControl_In winin; // Control of Inside of Window(s) (R/W)
    WindowControl_Out winout; // Control of Outside of Windows & Inside of OBJ Window (R/W)

    Mosaic mosaic; // Mosaic Size (W)

    BlendControl bldcnt; // Color Special Effects Selection (R/W)
    BlendAlpha bldalpha; // Alpha Blending Coefficients (R/W)
    BlendY bldy; // Brightness (Fade-In/Out) Coefficient (W)
};