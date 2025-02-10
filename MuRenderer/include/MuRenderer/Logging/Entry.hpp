#ifndef MURENDERER_LOGGING_ENTRY_HPP
#define MURENDERER_LOGGING_ENTRY_HPP

#include "MuRenderer/Logging/LoggingLevel.hpp"
#include <chrono>


namespace murenderer
{
    struct Entry
    {
        LoggingLevel m_logLevel;
        std::wstring m_note;
        int m_sourceLine;
        const wchar_t* m_sourceFile;
        const wchar_t* m_sourceFunctionName;
        std::chrono::system_clock::time_point m_timestamp;
    };
}

#endif // !MURENDERER_LOGGING_ENTRY_HPP
