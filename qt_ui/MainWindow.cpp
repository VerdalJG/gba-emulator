#include "MainWindow.hpp"
#include "EmulatorHandler.hpp"

// Qt headers
#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QStatusBar>



// SDL headers
#include "SDL/SDLWidget.hpp"

MainWindow::MainWindow(int width, int height, const char* windowTitle, QWidget *parent) :
    QMainWindow(parent),
    sdlWidget(new SDLWidget(this)),
    emulatorHandler(new EmulatorHandler()),
    emulatorThread(new QThread())
{
    SetupMenuBar();

    setCentralWidget(sdlWidget);
    setWindowTitle(windowTitle);
    resize(width, height);

    statusBar()->showMessage("Ready");

    // Move to thread because main thread is blocked with Qt event loop
    emulatorHandler->moveToThread(emulatorThread);

    // Connect thread start with emulator handler startup and loop
    connect(emulatorThread, &QThread::started, emulatorHandler, &EmulatorHandler::Startup);

    // Connect signal from GUI -> Emulator
    connect(this, &MainWindow::RequestLoadROM, emulatorHandler, 
        &EmulatorHandler::LoadROMRequest, Qt::QueuedConnection);

    // Connect shutdown event
    connect(qApp, &QApplication::aboutToQuit, [this] () {
        emulatorHandler->Shutdown();
        emulatorThread->quit();
        emulatorThread->wait();
    });

    // Connect finished update event to render
    connect(emulatorHandler, &EmulatorHandler::FrameReady, sdlWidget, &SDLWidget::Render);

    connect(emulatorHandler, &EmulatorHandler::StatusMessage, this, [this](const QString& message) {
    statusBar()->showMessage(message, 5000); // Show for 5 seconds
    });

    emulatorThread->start();
}

MainWindow::~MainWindow()
{
    delete emulatorHandler;
    delete emulatorThread;
}

void MainWindow::SetupMenuBar()
{
    // MainWindow already has a menu bar so we use it
    QMenuBar* mainMenuBar = menuBar();

    SetupFileMenu(mainMenuBar);
    SetupEmulationMenu(mainMenuBar);
    SetupOptionsMenu(mainMenuBar);
}

void MainWindow::SetupFileMenu(QMenuBar* mainMenuBar)
{
    QMenu* fileMenu = mainMenuBar->addMenu(tr("&File"));

    // Rom load action
    QAction* loadRomAction = new QAction(tr("Load ROM"), this);
    fileMenu->addAction(loadRomAction);

    QObject::connect(loadRomAction, &QAction::triggered, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this, // Window
            tr("Select GBA ROM File"), // Instruction for user
            "", // Initial directory
            "GBA ROM Files (*.gba);;All Files (*)"); // File filter

        if (!filePath.isEmpty()) 
        {
            emit RequestLoadROM(filePath); // Emit to emulator thread

            // TODO: Move to emulator thread
            // Load the ROM into emulator
            if (emulatorHandler->LoadROM(filePath.toStdString()))
            {
                emulatorHandler->PostStatus(QString("ROM loaded successfully: %1").arg(filePath));
                emulatorHandler->OnROMLoaded();
            }
        }
    });

    // Save and load state actions
    QMenu* saveStateMenu = new QMenu("Save State", this);
    QMenu* loadStateMenu = new QMenu("Load State", this);
    fileMenu->addMenu(saveStateMenu);
    fileMenu->addMenu(loadStateMenu);

    for (int i = 0; i < emulatorHandler->GetSaveStateSlots(); ++i)
    {
        QString slotLabel = QString("Slot %1").arg(i + 1);
        QAction* saveSlotAction = new QAction(slotLabel, this);
        saveStateMenu->addAction(saveSlotAction);

        QObject::connect(saveSlotAction, &QAction::triggered, [this, i]() {
            emulatorHandler->SaveStateToSlot(i);
        });

        QAction* loadSlotAction = new QAction(slotLabel, this);
        loadStateMenu->addAction(loadSlotAction);

        QObject::connect(loadSlotAction, &QAction::triggered, [this, i]() {
            emulatorHandler->LoadStateFromSlot(i);
        });
    }

    // Exit action
    QAction* exitAction = new QAction("Exit", this);
    fileMenu->addAction(exitAction);

    QObject::connect(exitAction, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::SetupEmulationMenu(QMenuBar* mainMenuBar)
{
    QMenu* emulationMenu = mainMenuBar->addMenu("Emulation");

    // Reset emulation action
    QAction* resetAction = new QAction("Reset", this);
    emulationMenu->addAction(resetAction);

    QObject::connect(resetAction, &QAction::triggered, [this]() {
        emulatorHandler->Shutdown();
        emulatorHandler->RunLoop();
        qDebug() << "Emulation has been reset";
    });

    // Pause and resume actions
    QAction* pauseAction = new QAction("Pause", this);
    emulationMenu->addAction(pauseAction);

    QObject::connect(pauseAction, &QAction::triggered, [this]() {
        emulatorHandler->PauseEmulation();
        qDebug() << "Emulation paused";
    });

    QAction* resumeAction = new QAction("Resume", this);
    emulationMenu->addAction(resumeAction);

    QObject::connect(resumeAction, &QAction::triggered, [this]() {
        emulatorHandler->ResumeEmulation();
    });

    QAction* shutdownGameAction = new QAction("Shutdown Game", this);
    emulationMenu->addAction(shutdownGameAction);

    QObject::connect(shutdownGameAction, &QAction::triggered, [this] () {
        emulatorHandler->Shutdown();
        qDebug() << "Emulation shutdown";
    });
}

void MainWindow::SetupOptionsMenu(QMenuBar* mainMenuBar)
{
    QMenu* optionsMenu = mainMenuBar->addMenu("Options");

    QAction* controlsAction = new QAction("Controls", this);
    optionsMenu->addAction(controlsAction);

    QObject::connect(controlsAction, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Controls setting", "Controls dialog is not implemented yet.");
    });
}
