#include "EmulatorHandler.hpp"
#include "EmulatorCore.hpp"

#include <QThread>

EmulatorHandler::EmulatorHandler() :
    emulatorCore(new EmulatorCore())
{

}

void EmulatorHandler::SaveStateToSlot(int slot)
{
}

void EmulatorHandler::LoadStateFromSlot(int slot)
{
}

bool EmulatorHandler::LoadROM(const std::string &romPath)
{
    if (emulatorCore->LoadROM(romPath))
    {
        isRunning = true;
        return true;
    }
    else
    {
        isRunning = false;
        return false;
    }
}

void EmulatorHandler::PauseEmulation()
{
    isRunning = false;
}

void EmulatorHandler::ResumeEmulation()
{
    isRunning = true;
}

void EmulatorHandler::RunLoop()
{
    while (isRunning)
    {
        emulatorCore->Update();
        emit FrameReady();
        QThread::msleep(16);
    }
}

void EmulatorHandler::Shutdown()
{
    isRunning = false;
    emulatorCore->Shutdown();
}