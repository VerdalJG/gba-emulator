#pragma once
#include <fstream>
#include <string>
#include <map>

enum class LogType
{
    Info,
    Warning,
    Error,
    MissingImplementation
};

class Logger 
{
public:
    Logger(const std::string& filename = "log.txt");
    ~Logger();
    void Log(const std::string& message, LogType logType, const std::string funcName = "");
    

private:
    std::map<LogType, std::string> logTypeStrings = 
    {
        {LogType::Info, "[INFO]: "},
        {LogType::Warning, "[WARNING]: "},
        {LogType::Error, "[ERROR]: "},
        {LogType::MissingImplementation, "[MissingImplementation]: "}
    };

    std::ofstream file;
    bool productionSafe = false;
};