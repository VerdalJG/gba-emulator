#include "EmulatorHandler.hpp"
#include "QtUtils.hpp"

#include "Core/EmulatorCore.hpp"
#include "Core/Memory/GBA_Memory.hpp"
#include "Core/GBA_PPU.hpp"

#include "Utils/Logger.hpp"

// std includes
#include <fstream>
#include <cstdint>
#include <vector>
#include <iostream>
#include <filesystem>

// QT includes
#include <QThread>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>

EmulatorHandler::EmulatorHandler() :
    logger(),
    emulatorCore(new EmulatorCore(&logger))
{
    auto statusUICallback = [this](const std::string& message)
    {
        PostStatus(QString::fromStdString(message));
    };
    
    emulatorCore->SetPostStatusCallback(statusUICallback);
}

EmulatorHandler::~EmulatorHandler() 
{
    delete emulatorCore;
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
        // std::cout << "CWD: " << std::filesystem::current_path() << "\n";
        // std::string biosPath = "bios/gba_bios.bin";

        std::string biosPath = 
            QCoreApplication::applicationDirPath().toStdString() + "/bios/gba_bios.bin";
            
        std::string biosPathMessage = "AppDir = '" + biosPath;    
        logger.Log(biosPathMessage, LogType::Info);
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

bool EmulatorHandler::LoadROM(const std::string& romPath)
{
    qDebug() << "Loading ROM from path: " << romPath;

    std::vector<uint8_t> romData;

    // Load file into romData
    if (!LoadFile(romPath, romData))
    {
        emit ErrorOccurred("File failed to load.");
        return false;
    }

    Q_ASSERT(emulatorCore && "EmulatorCore is null in LoadROM()");

    // Pass ROM data to the core
    if (!emulatorCore->LoadROM(romData)) 
    {
        emit ErrorOccurred("Core failed to load ROM.");
        return false;
    }

    return true;
}

void EmulatorHandler::PauseEmulation()
{
    isRunning = false;
}

void EmulatorHandler::ResumeEmulation()
{
    isRunning = true;
    RunLoop();
}

void EmulatorHandler::RunLoop()
{
    while (isRunning)
    {
        emulatorCore->RunFrame();
        RenderFrame();
    }
}

void EmulatorHandler::Shutdown()
{
    isRunning = false;
    emulatorCore->Shutdown();
}

void EmulatorHandler::LoadROMRequest(const QString& path)
{
    if (LoadROM(path.toStdString()))
    {
        PostStatus(QString("ROM loaded successfully: %1").arg(path));
        OnROMLoaded();
    }
}

void EmulatorHandler::RenderFrame() 
{
    const uint32_t* frameBuffer = emulatorCore->GetPPU().GetFrameBuffer();
    emit FrameReady(frameBuffer);
}

void EmulatorHandler::PostStatus(const QString& message, int seconds) {
  emit StatusMessage(message, seconds);
  qDebug() << message;
}

void EmulatorHandler::OnROMLoaded()
{
    emulatorCore->InitializeCPU();
    isRunning = true;
    RunLoop();
}
