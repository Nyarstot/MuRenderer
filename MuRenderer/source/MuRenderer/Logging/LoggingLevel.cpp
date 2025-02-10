#include "MuRenderer/Logging/LoggingLevel.hpp"


namespace murenderer
{
    std::wstring GetLoggingLevelName(LoggingLevel aLoggingLevel)
    {
        switch (aLoggingLevel)
        {
        case murenderer::LoggingLevel::LoggingLevel_Trace:
            return L"Trace";
        case murenderer::LoggingLevel::LoggingLevel_Debug:
            return L"Debug";
        case murenderer::LoggingLevel::LoggingLevel_Info:
            return L"Info";
        case murenderer::LoggingLevel::LoggingLevel_Warn:
            return L"Warning";
        case murenderer::LoggingLevel::LoggingLevel_Error:
            return L"Error";
        case murenderer::LoggingLevel::LoggingLevel_Fatal:
            return L"Fatal";
        default:
            return L"Unknown";
        }
    }
}