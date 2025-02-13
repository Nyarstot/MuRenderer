#ifndef MURENDERER_UTILS_FPSCOUNTER_HPP
#define MURENDERER_UTILS_FPSCOUNTER_HPP

#include "MuRenderer/System/SystemTypes.hpp"


namespace murenderer
{
    class FPSCounter final
    {
    private:
        double m_fps;
        double m_frameAccumulator;
        mu_size_t m_frameCounter;

        mu_lint_t m_frequency;
        mu_lint_t m_counter;
        mu_lint_t m_elapsed;
        mu_lint_t m_lastCounter;

    public:
        FPSCounter();
        ~FPSCounter();

        void QueryCounter();
        void BeginFrame();
        void EndFrame();

        double GetFPS() const;

    };
}

#endif // !MURENDERER_UTILS_FPSCOUNTER_HPP
