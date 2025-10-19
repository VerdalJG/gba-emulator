#include "Utils/Logger.hpp"

Logger::Logger(const std::string& filename)
: file(filename, std::ios::out | std::ios::trunc)
{
}

Logger::~Logger()
{
    file.close();
}

void Logger::Log(const std::string& message, LogType logType, const std::string funcName)
{
    if (file.is_open()) 
    {
        std::string fullMessage = logTypeStrings[logType] + message + " at: " + funcName;

        file << fullMessage << '\n';
    }
}