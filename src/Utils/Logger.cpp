#include "Utils/Logger.hpp"

#include <filesystem>
#include <iostream>

Logger::Logger(const std::string& filename)
{
    try {
        // Make an absolute path relative to the executable
        std::filesystem::path exePath = std::filesystem::current_path();
        std::filesystem::path logPath = exePath / filename;

        file.open(logPath, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + logPath.string());
        }
    } catch (const std::exception& e) {
        std::cerr << "Logger failed: " << e.what() << std::endl;
    }
}

Logger::~Logger()
{
    file.close();
}

void Logger::Log(const std::string& message, LogType logType, const std::string funcName)
{
    if (file.is_open()) 
    {
        std::string fullMessage = logTypeStrings[logType] + message;

        if (!funcName.empty()) fullMessage += " at: " + funcName;

        file << fullMessage << std::endl;
    }
    else
    {
        throw std::runtime_error("Failed to open log file");
    }
}