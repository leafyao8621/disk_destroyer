#include "app.h"

GDiskDestroyer::App::App(int *argc, char ***argv) {
    gtk_init(argc, argv);
    this->builder = gtk_builder_new();
    this->window.push_back(
        std::make_unique<MainWindow>(this)
    );
    this->window.back()->init();
}

void GDiskDestroyer::App::build_window(char *name, GError *error) {
    if (
        !gtk_builder_add_from_resource(
            this->builder,
            name,
            &error
        )) {
        throw Err::WINDOW_CREATE;
    }
}

GObject *GDiskDestroyer::App::get_object(char *name) {
    return
        gtk_builder_get_object(
            this->builder,
            name
        );
}

void GDiskDestroyer::App::operator()() {
    g_print("%s\n", "run");
    gtk_main();
}
