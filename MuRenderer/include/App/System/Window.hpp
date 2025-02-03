#ifndef APP_SYSTEM_WINDOW_HPP
#define APP_SYSTEM_WINDOW_HPP

#include <Windows.h>


namespace app
{
    struct WindowDesc
    {

    };

    class Window
    {
    private:
        LPCSTR m_title;
        HINSTANCE m_hinstance;
        HWND m_hwnd;

    protected:
        void CreateInitial(const LPCSTR& aName, int aWidth, int aHeight);

    public:
        Window(const LPCSTR& aName, int aWidth, int aHeight);
        ~Window();

        void Show();

    };
}

#endif // !APP_SYSTEM_WINDOW_HPP
