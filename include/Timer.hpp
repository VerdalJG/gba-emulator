#pragma once

#include <SDL3/SDL.h>

/// @brief 
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
    double GetGBA_FPS() const { return gbaFPS; }
    int GetCyclesToRunThisFrame() { return static_cast<int>(gbaCyclesPerFrame * timeScale);}
    void SetTimeScale(double scale) { timeScale = scale; }



private:
    const double gbaCyclesPerSecond = 16777216; // GBA CPU runs at 16.78 MHz
    const double gbaFPS = 59.73; // GBA runs at 59.73 FPS
    const double maxAccumulatedCycles = gbaCyclesPerFrame * 5; // Maximum cycles to accumulate before capping to prevent death spiral
    const int gbaCyclesPerFrame = static_cast<int>(gbaCyclesPerSecond / gbaFPS); // 280896

    Uint64 pcTickFrequency; 
    Uint64 currentTickCount;
    Uint64 previousTickCount;
    double deltaTime;
    double accumulatedCycles; // Used to accumulate cycles for the current frame
    double timeScale = 1.0; // Used for fast forwarding or slowing down the emulation
    double currentFPS; // Current FPS for the emulation
    
    
};