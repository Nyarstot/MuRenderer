#ifndef APP_CORE_APP_HPP
#define APP_CORE_APP_HPP

#include "App/System/Window.hpp"
#include "MuRenderer/Core/MultiGPURenderer.hpp"
#include "MuRenderer/Core/SingleGPURenderer.hpp"
#include "MuRenderer/Utils/FPSCounter.hpp"
#include <memory>


namespace app
{
    class App final
    {
    private:
        std::unique_ptr<Window> m_window;
        std::unique_ptr<murenderer::FPSCounter> m_fpsCounter;
        std::unique_ptr<murenderer::Renderer> m_renderer;
        bool m_shutdownRequired;

    public:
        App();
        ~App();

        void Run();

    };
}

#endif // !APP_CORE_APP_HPP
