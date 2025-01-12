#ifndef FRONTEND_APP_H_
#define FRONTEND_APP_H_

#include <vector>
#include <memory>
#include <list>

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
        GtkTextView *log_text_view;
        GtkTextBuffer *log_text_buffer;
        GtkEntry *block_entry;
        GtkFileChooserButton *device_file_chooser_button;
        GtkButton *run_button, *halt_button;
        DiskDestroyer::Config::RCConfig rc_config;
        char *pattern_compiled;
        size_t buf_size;
        std::string file_name;
        std::list<std::string> message;
        bool stop, stop_confirmed;
        GThread *worker;
        GMutex mutex_interface;
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
        void set_buf_size(size_t buf_size);
        size_t get_buf_size();
        void set_file_name(std::string file_name);
        std::string get_file_name();
        void set_log(char *log);
        void set_message(std::string message);
        void append_log();
        void run_success();
        void run_failure();
        bool get_stop();
        void set_stop_confirmed();
        void launch();
        void halt();
        App(int *argc, char ***argv);
        void init();
    public:
        void operator()();
    };
}

#endif
