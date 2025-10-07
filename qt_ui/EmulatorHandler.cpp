#include "EmulatorHandler.hpp"
#include "Core/EmulatorCore.hpp"
#include "Utils.hpp"
#include "Core/CPU/CPU_Memory.hpp"

// std includes
#include <fstream>
#include <cstdint>
#include <vector>


// QT includes
#include <QThread>
#include <QMessageBox>
#include <QDebug>

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
    std::vector<uint8_t> biosData;
    std::string biosPath = "bios/gba_bios.bin";
    bool loadedBios = LoadFile(biosPath, biosData);
    
    
    if (loadedBios && biosData.size() == BIOS_SIZE)
    {
        emulatorCore->SetUsingHLE(true);
        emulatorCore->LoadBIOS(biosData);
        qDebug() << "Loaded real BIOS\n";
    }
    else 
    {
        emulatorCore->SetUsingHLE(false);
        qDebug() << "Using HLE BIOS (No valid 'gba_bios.bin' found) \n ";
    }
}

bool EmulatorHandler::LoadROM(const std::string &romPath)
{
    qDebug() << "Loading ROM from path: " << romPath;

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