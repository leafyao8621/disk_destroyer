#include "../app.h"

GDiskDestroyer::App::MainWindow::MainWindow(App *app) :
    GDiskDestroyer::App::Window(app, "/com/example/gdide/gdide.glade") {
    this->app = app;
    this->window = app->get_object("window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void GDiskDestroyer::App::MainWindow::init() {

}
