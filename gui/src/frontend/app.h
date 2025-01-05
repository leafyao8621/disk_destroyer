#ifndef FRONTEND_APP_H_
#define FRONTEND_APP_H_

#include <vector>
#include <memory>

#include <gtk-3.0/gtk/gtk.h>

namespace GDiskDestroyer {
    class App {
        class Window {
        protected:
            App *app;
            GObject *window;
            GError *error;
        public:
            Window(App *app, char *name);
            ~Window() {};
            virtual void init() = 0;
        };
        class MainWindow : public Window {
        public:
            MainWindow(App *app);
            void init();
        };
        GtkBuilder *builder;
        std::vector<std::unique_ptr<Window> > window;
    public:
        enum Err {
            WINDOW_CREATE
        };
        App(int *argc, char ***argv);
        void build_window(char *name, GError *error);
        GObject *get_object(char *name);
        void operator()();
    };
}

#endif
