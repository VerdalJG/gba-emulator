#include "QtUtils.hpp"
#include <fstream>
#include <stdexcept>

// QT includes
#include <QThread>
#include <QMessageBox>
#include <QFile>
#include <QIODevice>

bool LoadFile(const std::string& path, std::vector<uint8_t>& buffer)
{
    QString filePath = QString::fromStdString(path);
    
    // Open file
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file: " << filePath;
        return false;
    }

    // Read data and check if empty
    QByteArray data = file.readAll();
    if (data.isEmpty())
    {
        qDebug() << "Failed to read file: " << filePath;
        return false;
    }

    buffer.assign(data.begin(), data.end()); // Put into vector
    return true;
}