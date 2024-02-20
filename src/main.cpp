// MIT License

// Copyright (c) 2024 Leandro Peres, aka "zschzen"

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Application.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

void
AppLoop(void* arg)
{
    Application &app = *reinterpret_cast<Application*>(arg);

#if __EMSCRIPTEN__
    if (!app.IsRunning())
    {
        emscripten_cancel_main_loop();
        return;
    }
#endif

    app.Input();
    app.Update();
    app.Render();
}

int
main(int argc, char* argv[])
{
    // Silence unused variable warnings
    (void)argc;
    (void)argv;

    Application app;
    app.Setup();

#if __EMSCRIPTEN__
    emscripten_set_main_loop_arg(AppLoop, &app, 0, 1);
#else
    while (app.IsRunning())
    {
        AppLoop(&app);
    }
#endif

    app.Destroy();

    return 0;
}