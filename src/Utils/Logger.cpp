#include "Utils/Logger.hpp"

#include <filesystem>
#include <iostream>
#include <windows.h>

std::filesystem::path GetExecutableDir()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
}

Logger::Logger(const std::string& filename)
{
    try 
    {
        // Make an absolute path relative to the executable directory
        std::filesystem::path baseDir = GetExecutableDir();
        std::filesystem::path logPath = baseDir / filename;

        file.open(logPath, std::ios::out | std::ios::trunc);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + logPath.string());
        }
    } 
    catch (const std::exception& e) 
    {
        if (productionSafe)
        {
            std::cerr << "Logger failed: " << e.what() << std::endl;
            file.open("fallback_log.txt", std::ios::out | std::ios::trunc);
        }
        else // Cause an error so we can immediately see it
        {
            throw std::runtime_error(std::string("Logger init failed: ") + e.what());
        }
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