#ifndef MURENDERER_BINDABLE_IBINDABLE_HPP
#define MURENDERER_BINDABLE_IBINDABLE_HPP


namespace murenderer
{
    enum class BindableType : mu_uint16_t
    {
        BindableType_None,
        BindableType_RSStage,
        BindableType_IAStage,
        BindableType_VSStage,
        BindableType_PSStage,
        BindableType_ConstBuf,
        BindableType_Count
    };

    class IBindable
    {
    public:
        virtual ~IBindable() = default;
        virtual void Bind() = 0;

    };
}

#endif // !MURENDERER_BINDABLE_IBINDABLE_HPP
