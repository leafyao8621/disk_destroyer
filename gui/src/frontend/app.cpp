#include <iostream>
#include <sstream>
#include <filesystem>

#include "../backend/config/config.h"
#include "../backend/config/parser.h"
#include "../backend/writer/writer.h"
#include "app.h"

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

void GDiskDestroyer::App::set_built_in() {
    gtk_combo_box_set_button_sensitivity(
        this->built_in_combo_box, GTK_SENSITIVITY_AUTO
    );
    gtk_combo_box_set_button_sensitivity(
        this->rc_combo_box, GTK_SENSITIVITY_OFF
    );
    gtk_editable_set_editable(GTK_EDITABLE(this->custom_entry), false);
}

void GDiskDestroyer::App::set_rc() {
    gtk_combo_box_set_button_sensitivity(
        this->built_in_combo_box, GTK_SENSITIVITY_OFF
    );
    gtk_combo_box_set_button_sensitivity(
        this->rc_combo_box, GTK_SENSITIVITY_AUTO
    );
    gtk_editable_set_editable(GTK_EDITABLE(this->custom_entry), false);
}

void GDiskDestroyer::App::set_custom() {
    gtk_combo_box_set_button_sensitivity(
        this->built_in_combo_box, GTK_SENSITIVITY_OFF
    );
    gtk_combo_box_set_button_sensitivity(
        this->rc_combo_box, GTK_SENSITIVITY_OFF
    );
    gtk_editable_set_editable(GTK_EDITABLE(this->custom_entry), true);
}

void built_in_radio_button_toggled(
    GtkToggleButton *toggle_button, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->set_built_in();
}

void rc_radio_button_toggled(
    GtkToggleButton *toggle_button, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->set_rc();
}

void custom_radio_button_toggled(
    GtkToggleButton *toggle_button, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->set_custom();
}

void GDiskDestroyer::App::set_pattern_compiled(char *pattern_compiled) {
    this->pattern_compiled = pattern_compiled;
}

char *GDiskDestroyer::App::get_pattern_compiled() {
    return this->pattern_compiled;
}

void GDiskDestroyer::App::set_log(char *log) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(this->log_text_buffer, &start);
    gtk_text_buffer_get_end_iter(this->log_text_buffer, &end);
    gtk_text_buffer_delete(this->log_text_buffer, &start, &end);
    gtk_text_buffer_insert(this->log_text_buffer, &start, log, -1);
}

void builtin_combo_box_changed(GtkComboBox *combo_box, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->set_pattern_compiled(
        (char*)
            DiskDestroyer::Config::config[
                gtk_combo_box_get_active(combo_box)
            ]
    );
    std::ostringstream oss;
    DiskDestroyer::Config::Parser::log(app->get_pattern_compiled(), oss);
    app->set_log((char*)oss.str().c_str());
}

void GDiskDestroyer::App::set_buf_size(size_t buf_size) {
    this->buf_size = buf_size;
}

size_t GDiskDestroyer::App::get_buf_size() {
    return this->buf_size;
}

void block_entry_editable_changed(GtkEditable *editable, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    ssize_t val = atoll(gtk_editable_get_chars(editable, 0, -1));
    if (val <= 0) {
        app->set_log((char*)"Invalid block size");
    } else {
        app->set_buf_size(val);
        app->set_log((char*)"Block size set");
    }
}

void GDiskDestroyer::App::set_file_name(std::string file_name) {
    this->file_name = file_name;
}

std::string GDiskDestroyer::App::get_file_name() {
    return this->file_name;
}

void device_file_chooser_button_file_set(
    GtkFileChooserButton *file_chooser_button, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->set_file_name(
        std::string(
            gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(file_chooser_button)
            )
        )
    );

    std::error_code ec;
    std::filesystem::space_info si =
        std::filesystem::space(app->get_file_name(), ec);
    if (si.capacity == 971184685056) {
        app->set_log((char*)"Invalid device");
    } else {
        std::ostringstream oss;
        oss << "Device set\n" << "Capacity " << si.capacity;
        app->set_log((char*)oss.str().c_str());
    }
}

void GDiskDestroyer::App::set_message(std::string message) {
    this->message = message;
}

void GDiskDestroyer::App::append_log() {
    g_mutex_lock(&this->mutex_interface);
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(this->log_text_buffer, &end);
    gtk_text_buffer_insert(
        this->log_text_buffer,
        &end,
        this->message.c_str(),
        -1
    );
    gtk_text_buffer_get_end_iter(this->log_text_buffer, &end);
    gtk_text_view_scroll_to_iter(
        this->log_text_view,
        &end,
        0,
        true,
        0,
        1
    );
    g_mutex_unlock(&this->mutex_interface);
}

int run_success_idle(gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->run_success();
    return 0;
}

int run_failure_idle(gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->run_failure();
    return 0;
}

void GDiskDestroyer::App::set_stop_confirmed() {
    this->stop_confirmed = true;
}

void *worker_callback(gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    try {
        DiskDestroyer::Writer writer(
            app->get_file_name(),
            app->get_buf_size(),
            app
        );
        writer.init();
        writer((char*)app->get_pattern_compiled());
        if (app->get_stop()) {
            app->set_stop_confirmed();
            g_print("ENd");
            return 0;
        }
        g_idle_add(run_success_idle, app);
    } catch (DiskDestroyer::Gen::Err) {
        g_idle_add(run_failure_idle, app);
    } catch (DiskDestroyer::Writer::Err) {
        g_idle_add(run_failure_idle, app);
    }
    app->set_stop_confirmed();
    g_print("End");
    return 0;
}

void GDiskDestroyer::App::launch() {
    this->stop = false;
    this->stop_confirmed = false;
    gtk_combo_box_set_button_sensitivity(
        this->built_in_combo_box, GTK_SENSITIVITY_OFF);
    gtk_combo_box_set_button_sensitivity(
        this->rc_combo_box, GTK_SENSITIVITY_OFF);
    gtk_editable_set_editable(
        GTK_EDITABLE(this->custom_entry), false);
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->run_button),
        false
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->built_in_radio_button),
        false
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->rc_radio_button),
        false
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->custom_radio_button),
        false
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->device_file_chooser_button),
        false
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->halt_button),
        true
    );
    this->set_log((char*)"");
    this->worker = g_thread_new("worker", worker_callback, this);
}

void run_button_clicked(GtkButton *button, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->launch();
}

void GDiskDestroyer::App::run_success() {
    gtk_combo_box_set_button_sensitivity(
        this->built_in_combo_box,
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->built_in_radio_button)
        ) ? GTK_SENSITIVITY_AUTO : GTK_SENSITIVITY_OFF
    );
    gtk_combo_box_set_button_sensitivity(
        this->rc_combo_box,
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->rc_radio_button)
        ) ? GTK_SENSITIVITY_AUTO : GTK_SENSITIVITY_OFF
    );
    gtk_editable_set_editable(
        GTK_EDITABLE(this->custom_entry),
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->built_in_radio_button)
        )
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->run_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->built_in_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->rc_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->custom_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->device_file_chooser_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->halt_button),
        false
    );
    this->set_log((char*)"Success");
    g_thread_join(this->worker);
    this->worker = 0;
}

void GDiskDestroyer::App::run_failure() {
    gtk_combo_box_set_button_sensitivity(
        this->built_in_combo_box,
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->built_in_radio_button)
        ) ? GTK_SENSITIVITY_AUTO : GTK_SENSITIVITY_OFF
    );
    gtk_combo_box_set_button_sensitivity(
        this->rc_combo_box,
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->rc_radio_button)
        ) ? GTK_SENSITIVITY_AUTO : GTK_SENSITIVITY_OFF
    );
    gtk_editable_set_editable(
        GTK_EDITABLE(this->custom_entry),
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->built_in_radio_button)
        )
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->run_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->built_in_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->rc_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->custom_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->device_file_chooser_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->halt_button),
        false
    );
    this->set_log((char*)"Failure");
    g_thread_join(this->worker);
    this->worker = 0;
}

bool GDiskDestroyer::App::get_stop() {
    return this->stop;
}

void GDiskDestroyer::App::halt() {
    g_print("halt\n");
    this->stop = true;
    for (; !this->stop_confirmed;) {
        for (; gtk_events_pending(); gtk_main_iteration());
    }
    g_print("halt confirm\n");
    gtk_combo_box_set_button_sensitivity(
        this->built_in_combo_box,
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->built_in_radio_button)
        ) ? GTK_SENSITIVITY_AUTO : GTK_SENSITIVITY_OFF
    );
    gtk_combo_box_set_button_sensitivity(
        this->rc_combo_box,
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->rc_radio_button)
        ) ? GTK_SENSITIVITY_AUTO : GTK_SENSITIVITY_OFF
    );
    gtk_editable_set_editable(
        GTK_EDITABLE(this->custom_entry),
        gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(this->built_in_radio_button)
        )
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->run_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->built_in_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->rc_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->custom_radio_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->device_file_chooser_button),
        true
    );
    gtk_widget_set_sensitive(
        GTK_WIDGET(this->halt_button),
        false
    );
    this->set_log((char*)"Halted");
    g_print("join");
    g_thread_join(this->worker);
    this->worker = 0;
}

void halt_button_clicked(GtkButton *button, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->halt();
}

GDiskDestroyer::App::App(int *argc, char ***argv) {
    this->buf_size = 4096;
    this->pattern_compiled = (char*)"";
    this->file_name = "";
    this->stop = true;
    this->stop_confirmed = true;
    this->worker = 0;
    gtk_init(argc, argv);
    this->builder = gtk_builder_new();
    this->build_window((char*)"/com/example/gdide/gdide.glade");
    this->window = GTK_WINDOW(this->get_object((char*)"window"));
    g_signal_connect(
        this->window,
        "destroy",
        gtk_main_quit,
        this
    );
    this->built_in_radio_button =
        GTK_RADIO_BUTTON(this->get_object((char*)"built_in_radio_button"));
    this->rc_radio_button =
        GTK_RADIO_BUTTON(this->get_object((char*)"rc_radio_button"));
    this->custom_radio_button =
        GTK_RADIO_BUTTON(this->get_object((char*)"custom_radio_button"));
    this->built_in_combo_box =
        GTK_COMBO_BOX(this->get_object((char*)"built_in_combo_box"));
    this->rc_combo_box =
        GTK_COMBO_BOX(this->get_object((char*)"rc_combo_box"));
    this->custom_entry =
        GTK_ENTRY(this->get_object((char*)"custom_entry"));
    this->log_text_view =
        GTK_TEXT_VIEW(
            this->get_object((char*)"log_text_view")
        );
    this->block_entry =
        GTK_ENTRY(this->get_object((char*)"block_entry"));
    this->device_file_chooser_button =
        GTK_FILE_CHOOSER_BUTTON(
            this->get_object((char*)"device_file_chooser_button")
        );
    this->run_button = GTK_BUTTON(this->get_object((char*)"run_button"));
    this->halt_button = GTK_BUTTON(this->get_object((char*)"halt_button"));
}

void GDiskDestroyer::App::init() {
    GtkListStore *list =
        GTK_LIST_STORE(this->get_object((char*)"rc_pattern_list_store"));
    GtkTreeIter iter;
    for (auto &i : this->rc_config) {
        gtk_list_store_append(list, &iter);
        gtk_list_store_set(list, &iter, 0, i.first.c_str(), -1);
    }
    this->log_text_buffer =
        gtk_text_view_get_buffer(
            this->log_text_view
        );
    g_signal_connect(
        this->built_in_radio_button,
        "toggled",
        G_CALLBACK(built_in_radio_button_toggled),
        this
    );
    g_signal_connect(
        this->rc_radio_button,
        "toggled",
        G_CALLBACK(rc_radio_button_toggled),
        this
    );
    g_signal_connect(
        this->custom_radio_button,
        "toggled",
        G_CALLBACK(custom_radio_button_toggled),
        this
    );
    g_signal_connect(
        this->built_in_combo_box,
        "changed",
        G_CALLBACK(builtin_combo_box_changed),
        this
    );
    g_signal_connect(
        GTK_EDITABLE(this->block_entry),
        "changed",
        G_CALLBACK(block_entry_editable_changed),
        this
    );
    g_signal_connect(
        this->device_file_chooser_button,
        "file-set",
        G_CALLBACK(device_file_chooser_button_file_set),
        this
    );
    g_signal_connect(
        this->run_button,
        "clicked",
        G_CALLBACK(run_button_clicked),
        this
    );
    g_signal_connect(
        this->halt_button,
        "clicked",
        G_CALLBACK(halt_button_clicked),
        this
    );
    gtk_window_present(this->window);
}

void GDiskDestroyer::App::operator()() {
    gtk_main();
}
