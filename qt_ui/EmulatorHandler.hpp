#pragma once

#include <QObject>
#include <string>
#include "Utils/Logger.hpp"

class EmulatorCore;

class EmulatorHandler : public QObject
{
    Q_OBJECT

public:
    EmulatorHandler();
    ~EmulatorHandler();
    int GetSaveStateSlots() const { return saveStateSlots; }
    void SaveStateToSlot(int slot);
    void LoadStateFromSlot(int slot);
    void Startup();
    bool LoadROM(const std::string& romPath);
    void PauseEmulation();
    void ResumeEmulation();

    void RenderFrame();
    void PostStatus(const QString& message, int seconds = 5);
    void OnROMLoaded();

public slots:
    void RunLoop();
    void Shutdown();
    void LoadROMRequest(const QString& path);

signals:
    void FrameReady(const uint32_t* frameBuffer);
    void StatusMessage(const QString& message, int seconds = 5);
    void ROMLoaded(const QString& path);
    void ErrorOccurred(const QString& errorMessage);


private:
    Logger logger;
    EmulatorCore* emulatorCore;
    const int saveStateSlots = 9;
    bool isRunning = false;
    bool shouldLoadRealBios = true;
};