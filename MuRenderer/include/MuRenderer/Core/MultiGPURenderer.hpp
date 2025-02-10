#ifndef MURENDERER_CORE_MULTIGPURENDERER_HPP
#define MURENDERER_CORE_MULTIGPURENDERER_HPP

#include "MuRenderer/Core/Renderer.hpp"


namespace murenderer
{
    class MultiGPURenderer final : public Renderer
    {
    private:


    public:
        MultiGPURenderer();
        ~MultiGPURenderer();

        virtual void OnInit()   override;
        virtual void OnUpdate() override;

        virtual void CreateRootSignature() override;
        virtual void LoadPipeline() override;
        virtual void LoadAsset() override;
        virtual void CreateCommandList() override;
        virtual void PopulateCommandList() override;
        virtual void ExecuteCommandList() override;
        virtual void MoveToNextFrame() override;

    };
}

#endif // !MURENDERER_CORE_MULTIGPURENDERER_HPP
