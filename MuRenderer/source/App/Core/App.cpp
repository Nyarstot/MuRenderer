#include "pch.hpp"

#include "App/Core/App.hpp"
#include <iostream>


namespace app
{
    App::App() : m_shutdownRequired(false)
    {
        m_window = std::make_unique<Window>("AppName", 1280, 720);
        m_fpsCounter = std::make_unique<murenderer::FPSCounter>();
        m_renderer = std::make_unique<murenderer::MultiGPURenderer>();
    }

    App::~App()
    {
    }

    void App::Run()
    {
        m_window->Show();

        m_fpsCounter->QueryCounter();
        while (!m_shutdownRequired) {
            m_fpsCounter->BeginFrame();

            m_window->PickMessages();

            m_fpsCounter->EndFrame();
        }
    }
}