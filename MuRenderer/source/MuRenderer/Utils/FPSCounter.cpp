#include "pch.hpp"
#include "MuRenderer/Utils/FPSCounter.hpp"


namespace murenderer
{
    FPSCounter::FPSCounter()
    {
        m_fps = 0.0f;
        m_frameAccumulator = 0.0f;
        m_frameCounter = 0;
    }

    FPSCounter::~FPSCounter()
    {
    }

    void FPSCounter::QueryCounter()
    {
        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_counter);
    }

    void FPSCounter::BeginFrame()
    {
        m_lastCounter = m_counter;
        QueryPerformanceCounter(&m_counter);
        m_elapsed.QuadPart = m_counter.QuadPart - m_lastCounter.QuadPart;
        double currFrameTime = static_cast<double>(m_elapsed.QuadPart) / static_cast<double>(m_frequency.QuadPart);
        m_frameCounter++;
        m_frameAccumulator += currFrameTime;
    }

    void FPSCounter::EndFrame()
    {
        if (m_frameCounter == 15) {
            double frameTimeAvg = m_frameAccumulator / m_frameCounter;
            double fpsAvg = 1.0f / frameTimeAvg;

            m_fps = fpsAvg;
            m_frameAccumulator = frameTimeAvg;
            m_frameCounter = 1;
        }
    }

    double FPSCounter::GetFPS() const
    {
        return m_fps;
    }
}