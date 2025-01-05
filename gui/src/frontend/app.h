#ifndef FRONTEND_APP_H_
#define FRONTEND_APP_H_

#include <vector>
#include <memory>

#include <gtk-3.0/gtk/gtk.h>

#include "../backend/config/rc_config.h"

namespace GDiskDestroyer {
    class App {
        GtkBuilder *builder;
        GError *error;
        GtkWindow *window;
        GtkRadioButton *built_in_radio_button;
        GtkRadioButton *rc_radio_button;
        GtkRadioButton *custom_radio_button;
        GtkComboBox *built_in_combo_box;
        GtkComboBox *rc_combo_box;
        GtkEntry *custom_entry;
        DiskDestroyer::Config::RCConfig rc_config;
        void build_window(char *name);
        GObject *get_object(char *name);
    public:
        enum Err {
            WINDOW_CREATE
        };
        void set_built_in();
        void set_rc();
        void set_custom();
        App(int *argc, char ***argv);
        void init();
    public:
        void operator()();
    };
}

#endif
