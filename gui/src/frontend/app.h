#ifndef FRONTEND_APP_H_
#define FRONTEND_APP_H_

#include <vector>
#include <memory>

#include <gtk-3.0/gtk/gtk.h>

namespace GDiskDestroyer {
    class App {
        GtkBuilder *builder;
        GError *error;
        GObject *window;
    public:
        enum Err {
            WINDOW_CREATE
        };
        App(int *argc, char ***argv);
    private:
        void build_window(char *name);
        GObject *get_object(char *name);
    public:
        void operator()();
    };
}

#endif
