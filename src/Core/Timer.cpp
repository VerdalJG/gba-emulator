#include "Timer.hpp"
//#include <SDL3/SDL.h>

// Timer::Timer() :
// currentTickCount(0),
// deltaTime(0),
// accumulatedCycles(0),
// timeScale(1.0),
// currentFPS(0.0)
// {
//     pcTickFrequency = SDL_GetPerformanceFrequency();
//     previousTickCount = SDL_GetPerformanceCounter();
// }

// void Timer::Update()
// {
//     // Get elapsed ticks
//     //currentTickCount = SDL_GetPerformanceCounter();
//     Uint64 currentTicksElapsed = currentTickCount - previousTickCount;

//     // Convert to microseconds to prevent losing precision due to floating point values
//     Uint64 elapsedMicroseconds = currentTicksElapsed * 1000000 / pcTickFrequency;

//     // Calculate delta time in seconds
//     deltaTime = static_cast<double>(elapsedMicroseconds) / 1000000;

//     // Accumulate cycles
//     accumulatedCycles += deltaTime * gbaCyclesPerSecond * timeScale;

//     HandleDeathSpiral();

//     // Calculate current FPS
//     currentFPS = 1.0 / deltaTime; 

//     // Update previous tick count
//     previousTickCount = currentTickCount;
// }

// bool Timer::ShouldRunNewFrame()
// {
//     if (accumulatedCycles >= gbaCyclesPerFrame)
//     {
//         accumulatedCycles -= gbaCyclesPerFrame; // Subtract the cycles for one frame
//         return true; // Enough time has passed to run a new frame
//     }

//     return false;
// }

// void Timer::HandleDeathSpiral()
// {
//     if (accumulatedCycles > maxAccumulatedCycles)
//     {
//         accumulatedCycles = maxAccumulatedCycles; // Cap the accumulated time to prevent death spiral
//     }
//     else if (accumulatedCycles < 0)
//     {
//         accumulatedCycles = 0; // Prevent negative accumulated time
//     }
// }

