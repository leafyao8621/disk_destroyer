#include "app.h"

GDiskDestroyer::App::App(int *argc, char ***argv) {
    gtk_init(argc, argv);
    this->builder = gtk_builder_new();
    this->build_window("/com/example/gdide/gdide.glade");
    this->window = this->get_object("window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_present(GTK_WINDOW(this->window));
}

void GDiskDestroyer::App::build_window(char *name) {
    if (
        !gtk_builder_add_from_resource(
            this->builder,
            name,
            &this->error
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
    gtk_main();
}
