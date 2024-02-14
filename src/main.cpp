#include "app/Application.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

void
AppLoop(void* arg) {
    Application& app = *reinterpret_cast<Application*>(arg);

    if (!app.IsRunning()) {
#if __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        return;
    }

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
    while (app.IsRunning()) {
        AppLoop(&app);
    }
#endif

    app.Destroy();

    return 0;
}