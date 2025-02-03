#include <iostream>

#include "App/System/Window.hpp"


int main(int argc, char** argv)
{
    app::Window mainWindow("name", 1280, 720);
    mainWindow.Show();

    while (true) {

    }

    return 0;
}