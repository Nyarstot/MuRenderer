#ifndef MURENDERER_BINDABLE_IBINDABLE_HPP
#define MURENDERER_BINDABLE_IBINDABLE_HPP


namespace murenderer
{
    class IBindable
    {
    public:
        virtual ~IBindable() = default;
        virtual void Bind() = 0;

    };
}

#endif // !MURENDERER_BINDABLE_IBINDABLE_HPP
