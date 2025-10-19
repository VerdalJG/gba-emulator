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
    ~EmulatorHandler() = default;
    int GetSaveStateSlots() const { return saveStateSlots; }
    void SaveStateToSlot(int slot);
    void LoadStateFromSlot(int slot);
    void Startup();
    bool LoadROM(const std::string& romPath);
    void PauseEmulation();
    void ResumeEmulation();
    void PostStatus(const QString& message);

public slots:
    void RunLoop();
    void Shutdown();

signals:
    void FrameReady();
    void StatusMessage(const QString& message);


private:
    Logger logger;
    EmulatorCore* emulatorCore;
    const int saveStateSlots = 9;
    bool isRunning = false;
};