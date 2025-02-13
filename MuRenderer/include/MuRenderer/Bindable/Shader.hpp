#ifndef MURENDERER_BINDABLE_SHADER_HPP
#define MURENDERER_BINDABLE_SHADER_HPP

#include "MuRenderer/Bindable/IBindable.hpp"


namespace murenderer
{
    enum class ShaderType : mu_uint16_t
    {
        ShaderType_VertexShader,
        ShaderType_PixelShader,
        ShaderType_Count
    };

    struct MU_SHADER_DESC
    {
        mu_wstring_t m_name;
        mu_wstring_t m_path;
        mu_wstring_t m_entryPoint;
        mu_wstring_t m_target;
        ShaderType m_shaderType;
    };

    class ShaderBase : public IBindable
    {
    protected:
        MU_SHADER_DESC m_shaderDesc;

    public:
        ShaderBase(const mu_wstring_t& aName, const mu_wstring_t& aPath, BindableType aBindableType);
        virtual ~ShaderBase() = default;

        virtual void Compile() = 0;
        virtual void Create() = 0;
        virtual void CreateAndCompile() = 0;
        virtual void Bind() noexcept = 0;

        mu_wstring_t GetName() const;
        mu_wstring_t GetPath() const;

    };

    class VertexShader final : public ShaderBase
    {
    private:
        ComPtr<ID3DBlob> m_shaderByteCode;

    };

    class PixelShader final : public ShaderBase
    {
    private:

    };
}

#endif // !MURENDERER_BINDABLE_SHADER_HPP
