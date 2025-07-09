#include "MainWindow.hpp"
#include <iostream>
#include <QApplication>


int bam(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //Create the main window
    //MainWindow mainWindow;
    //mainWindow.show();

    printf("Welcome to the GBA Emulator!\n");

    // Run the application event loop
    return app.exec();
    //return 0;
}

void printSomething()
{
    printf("This is a test function.\n");
}