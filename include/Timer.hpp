#pragma once

#include <SDL3/SDL.h>

class Timer
{
public:
    Timer();
    ~Timer() = default;

    void Update();

    bool ShouldRunNewFrame();
    void HandleDeathSpiral();


    // Getters and setters
    double GetDeltaTime() const { return deltaTime; }
    double GetCurrentFPS() const { return currentFPS; }
    double GetTimeScale() const { return timeScale; }
    double GetGBA_FPS() const { return GBA_FPS; }
    int GetCyclesToRunThisFrame() const { return static_cast<int>(accumulatedCycles); }
    void SetTimeScale(double scale) { timeScale = scale; }



private:
    const double GBA_CYCLES_PER_SECOND = 16777216; // GBA CPU runs at 16.78 MHz
    const double GBA_FPS = 59.73; // GBA runs at 59.73 FPS
    const int GBA_CYCLES_PER_FRAME = static_cast<int>(GBA_CYCLES_PER_SECOND / GBA_FPS); // 280896

    Uint64 pcTickFrequency; 
    Uint64 currentTickCount;
    Uint64 previousTickCount;
    double deltaTime;
    double accumulatedTime;
    double maxAccumulatedTime = 1.0 / 15; 
    double accumulatedCycles = 0; // Used to accumulate cycles for the current frame
    double timeScale = 1.0; // Used for fast forwarding or slowing down the emulation
    double currentFPS = 0.0; // Current FPS for the emulation
    
    
};