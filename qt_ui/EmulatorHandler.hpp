#pragma once

#include <QObject>
#include <string>

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
    bool LoadROM(const std::string& romPath);
    void PauseEmulation();
    void ResumeEmulation();

public slots:
    void RunLoop();
    void Shutdown();

signals:
    void FrameReady();

private:
    EmulatorCore* emulatorCore;
    const int saveStateSlots = 9;
    bool isRunning = false;
};