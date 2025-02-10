#ifndef MURENDERER_LOGGING_ENTRYBUILDER_HPP
#define MURENDERER_LOGGING_ENTRYBUILDER_HPP

#include "MuRenderer/Logging/Entry.hpp"


namespace murenderer
{
    struct ILoggingChannel;
    class EntryBuilder final : public Entry
    {
    private:


    private:
        ILoggingChannel* Destination;


    public:
        EntryBuilder(const wchar_t* aSourceFile, const wchar_t* aSourceFunctionName, int aSourceLine);
        ~EntryBuilder();

        EntryBuilder& Note();
        EntryBuilder& Level(LoggingLevel aLoggingLevel);
        EntryBuilder& Channel(ILoggingChannel* aChannel);

    };
}

#endif // !MURENDERER_LOGGING_ENTRYBUILDER_HPP
