#include "EmulatorHandler.hpp"
#include "Core/EmulatorCore.hpp"
#include "Utils.hpp"

// std includes
#include <fstream>
#include <cstdint>
#include <vector>


// QT includes
#include <QThread>
#include <QMessageBox>

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

void EmulatorHandler::Startup()
{
    std::string path = "bios/gba_bios.bin";
    std::vector<uint8_t> biosData;
    
    if (LoadFile(path, biosData))
    {
        emulatorCore->LoadBIOS(biosData);
    }
}

bool EmulatorHandler::LoadROM(const std::string &romPath)
{
    printf("Loading ROM from path: %s\n", romPath.c_str());

    std::vector<uint8_t> romData;

    if (!LoadFile(romPath, romData))
    {
        return false; // Error logging done inside LoadFile()
    }

    // Pass ROM data to the core
    if (!emulatorCore->LoadROM(romData)) 
    {
        QMessageBox::critical(nullptr, "Error", "Core failed to load ROM.");
        return false;
    }

    //isRunning = true;
    return true;
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