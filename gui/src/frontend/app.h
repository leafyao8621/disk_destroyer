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
        GtkTextBuffer *log_text_buffer;
        DiskDestroyer::Config::RCConfig rc_config;
        bool ready;
        char *pattern_compiled;
        void build_window(char *name);
        GObject *get_object(char *name);
    public:
        enum Err {
            WINDOW_CREATE
        };
        void set_built_in();
        void set_rc();
        void set_custom();
        void set_pattern_compiled(char *pattern_compiled);
        char *get_pattern_compiled();
        void set_log(char *log);
        void append_log();
        App(int *argc, char ***argv);
        void init();
    public:
        void operator()();
    };
}

#endif
