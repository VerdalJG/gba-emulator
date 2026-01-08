#include "EmulatorHandler.hpp"
#include "Core/EmulatorCore.hpp"
#include "QtUtils.hpp"
#include "Core/GBA_Memory.hpp"
#include "Utils/Logger.hpp"

// std includes
#include <fstream>
#include <cstdint>
#include <vector>

// QT includes
#include <QThread>
#include <QMessageBox>
#include <QDebug>

EmulatorHandler::EmulatorHandler() :
    logger(),
    emulatorCore(new EmulatorCore(&logger))
{
    auto statusUICallback = [this](const std::string& message)
    {
        emit StatusMessage(QString::fromStdString(message));
    };
    
    emulatorCore->SetPostStatusCallback(statusUICallback);
}

void EmulatorHandler::SaveStateToSlot(int slot)
{
}

void EmulatorHandler::LoadStateFromSlot(int slot)
{
}

void EmulatorHandler::Startup()
{
    if (shouldLoadRealBios)
    {
        std::vector<uint8_t> biosData;
        std::string biosPath = "bios/gba_bios.bin";
        bool loadedBios = LoadFile(biosPath, biosData);

        Q_ASSERT(emulatorCore && "EmulatorCore is null in Startup()");
        
        if (loadedBios && biosData.size() == BIOS_SIZE)
        {
            emulatorCore->SetUsingHLE(false);
            emulatorCore->LoadBIOS(biosData);
            PostStatus("Loaded real BIOS");
        }
        else 
        {
            emulatorCore->SetUsingHLE(true);
            PostStatus("Using HLE BIOS (No valid 'gba_bios.bin' found)");
        }
    }
    else 
    {
        emulatorCore->SetUsingHLE(true);
        PostStatus("Using HLE BIOS");
    }
}

bool EmulatorHandler::LoadROM(const std::string &romPath)
{
    qDebug() << "Loading ROM from path: " << romPath;

    std::vector<uint8_t> romData;

    if (!LoadFile(romPath, romData))
    {
        QMessageBox::critical(nullptr, "Error", "File failed to load.");
        return false; // Error logging done inside LoadFile()
    }

    Q_ASSERT(emulatorCore && "EmulatorCore is null in LoadROM()");

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
        PostStatus("RunningLoop!");
        emulatorCore->Step();
        emit FrameReady();
        QThread::msleep(16); // This is practically Vsync
    }
}

void EmulatorHandler::Shutdown()
{
    isRunning = false;
    emulatorCore->Shutdown();
}

void EmulatorHandler::PostStatus(const QString &message)
{
    emit StatusMessage(message);
    qDebug() << message;
}
