#include "Timer.hpp"
#include <SDL3/SDL.h>

Timer::Timer() :
currentTickCount(0),
deltaTime(0),
accumulatedTime(0)
{
    pcTickFrequency = SDL_GetPerformanceFrequency();
    previousTickCount = SDL_GetPerformanceCounter();
}

void Timer::Update()
{
    // Get elapsed ticks
    currentTickCount = SDL_GetPerformanceCounter();
    Uint64 currentTicksElapsed = currentTickCount - previousTickCount;

    // Convert to microseconds to prevent losing precision due to floating point values
    Uint64 elapsedMicroseconds = currentTicksElapsed * 1000000 / pcTickFrequency;

    // Calculate delta time in seconds
    deltaTime = static_cast<double>(elapsedMicroseconds) / 1000000;

    // Accumulate time
    accumulatedTime += deltaTime * timeScale;

    HandleDeathSpiral();

    // Calculate current FPS
    currentFPS = 1.0 / deltaTime; 

    // Update previous tick count
    previousTickCount = currentTickCount;
}

bool Timer::ShouldRunNewFrame()
{
    if (accumulatedTime >= 1 / GBA_FPS)
    {
        accumulatedTime -= 1 / GBA_FPS; // Subtract the time for one frame
        return true; // Enough time has passed to run a new frame
    }

    return false;
}

void Timer::HandleDeathSpiral()
{
    if (accumulatedTime > maxAccumulatedTime)
    {
        accumulatedTime = maxAccumulatedTime; // Cap the accumulated time to prevent death spiral
    }
    else if (accumulatedTime < 0)
    {
        accumulatedTime = 0; // Prevent negative accumulated time
    }
}

