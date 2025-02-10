#ifndef MURENDERER_LOGGING_LOGGINGCHANNEL_HPP
#define MURENDERER_LOGGING_LOGGINGCHANNEL_HPP


namespace murenderer
{
    struct Entry;
    class ILoggingChannel
    {
    public:
        virtual ~ILoggingChannel() = default;
        virtual void Submit(Entry& aEntry) = 0;

    };
}

#endif // !MURENDERER_LOGGING_LOGGINGCHANNEL_HPP
