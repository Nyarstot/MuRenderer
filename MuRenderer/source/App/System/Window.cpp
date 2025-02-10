#include "App/System/Window.hpp"
#include <stdexcept>

namespace app
{
    LRESULT CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message)
        {
        case WM_KEYDOWN:
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void Window::CreateInitial(const LPCSTR& aName, int aWidth, int aHeight)
    {
        WNDCLASSEX wc;
        wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc      = AppWndProc;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;
        wc.hInstance        = m_hinstance;
        wc.hIcon            = LoadIcon(nullptr, IDI_WINLOGO);
        wc.hIconSm          = wc.hIcon;
        wc.hCursor          = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground    = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wc.lpszMenuName     = nullptr;
        wc.lpszClassName    = m_title;
        wc.cbSize           = sizeof(WNDCLASSEX);

        HRESULT result = RegisterClassEx(&wc);
        if (FAILED(result)) {
            std::runtime_error("Failed to register window!");
        }

        auto screenWidth = aWidth;
        auto screenHeight = aHeight;

        RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;
        auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

        m_hwnd = CreateWindowEx(
            WS_EX_APPWINDOW,
            m_title,
            m_title,
            dwStyle,
            posX, posY,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr, nullptr, m_hinstance, nullptr
        );

        if (m_hwnd == NULL) {
            std::runtime_error("Failed to create window");
        }
    }

    Window::Window(const LPCSTR& aName, int aWidth, int aHeight) : m_title(aName), m_hinstance(GetModuleHandle(nullptr))
    {
        CreateInitial(aName, aWidth, aHeight);
    }

    Window::~Window()
    {
        DestroyWindow(m_hwnd);
    }

    void Window::Show()
    {
        ShowWindow(m_hwnd, SW_SHOWDEFAULT);
        SetForegroundWindow(m_hwnd);
        SetFocus(m_hwnd);
        ShowCursor(true);
    }

    void Window::PickMessages()
    {
        MSG msg = { 0 };
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            //if (msg.message == WM_QUIT)
            //    break;
        }
    }
}