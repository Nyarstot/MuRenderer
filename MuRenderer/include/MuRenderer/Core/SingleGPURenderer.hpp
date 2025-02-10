#ifndef MURENDERER_CORE_SINGLEGPURENDERER_HPP
#define MURENDERER_CORE_SINGLEGPURENDERER_HPP

#include "MuRenderer/Core/Renderer.hpp"


namespace murenderer
{
    class SingleGPURenderer final : public Renderer
    {
    private:


    public:
        SingleGPURenderer();
        ~SingleGPURenderer();

        virtual void CreateRootSignature() override;
        virtual void LoadPipeline() override;
        virtual void LoadAsset() override;
        virtual void CreateCommandList() override;
        virtual void PopulateCommandList() override;
        virtual void ExecuteCommandList() override;
        virtual void MoveToNextFrame() override;

    };
}

#endif // !MURENDERER_CORE_SINGLEGPURENDERER_HPP
