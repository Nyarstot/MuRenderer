#include "App/Core/App.hpp"


namespace app
{
    App::App() : m_shutdownRequired(false)
    {
        m_window = std::make_unique<Window>("AppName", 1280, 720);
        m_renderer = std::make_unique<murenderer::MultiGPURenderer>();
    }

    App::~App()
    {
    }

    void App::Run()
    {
        m_window->Show();

        while (!m_shutdownRequired) {
            m_window->PickMessages();
        }
    }
}