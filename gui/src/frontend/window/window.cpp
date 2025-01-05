#include "../app.h"

GDiskDestroyer::App::Window::Window(App *app, char *name) {
    try {
        app->build_window(name, this->error);
    } catch (Err err) {
        g_printerr("%s\n", this->error->message);
    }
}
