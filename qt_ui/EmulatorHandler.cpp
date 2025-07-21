#include "EmulatorHandler.hpp"
#include "EmulatorCore.hpp"

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

bool EmulatorHandler::LoadROM(const std::string &romPath)
{
    printf("Loading ROM from path: %s\n", romPath.c_str());

    // Open the ROM file in binary mode and read its contents from the end of the file (ate = at the end)
    std::ifstream romFile(romPath, std::ios::binary | std::ios::ate);

    if (!romFile.is_open())
    {
        QMessageBox::critical(nullptr, "Error", "Failed to open ROM file. Check the file path.");
        return false;
    }

    // Get the size of the file by reading the position at the end of file
    std::streamsize fileSize = romFile.tellg();

    if (fileSize <= 0)
    {
        QMessageBox::critical(nullptr, "Error", "ROM file appears to be empty or unreadable.");
        return false;
    }

    // Go back to the beginning of the file
    romFile.seekg(0, std::ios::beg);

    
    std::vector<uint8_t> romData(fileSize);
    if (!romFile.read(reinterpret_cast<char*>(romData.data()), fileSize))
    {
        QMessageBox::critical(nullptr, "Error", "Failed to read ROM file. Check the file format.");
        return false;
    }

    // Pass ROM data to the core
    if (!emulatorCore->LoadROM(romData)) {
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