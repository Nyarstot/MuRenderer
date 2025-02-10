#ifndef MURENDERER_LOGGING_LOGGINGLEVEL_HPP
#define MURENDERER_LOGGING_LOGGINGLEVEL_HPP

#include <string>

namespace murenderer
{
    enum class LoggingLevel
    {
        LoggingLevel_Trace,
        LoggingLevel_Debug,
        LoggingLevel_Info,
        LoggingLevel_Warn,
        LoggingLevel_Error,
        LoggingLevel_Fatal
    };

    std::wstring GetLoggingLevelName(LoggingLevel aLoggingLevel);
}

#endif // !MURENDERER_LOGGING_LOGGINGLEVEL_HPP
