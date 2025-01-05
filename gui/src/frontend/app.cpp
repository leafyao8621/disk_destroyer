#include <iostream>
#include <sstream>

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
    app->set_log((char*)"Device set");
}

void GDiskDestroyer::App::append_log() {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(this->log_text_buffer, &end);
    gtk_text_buffer_insert(
        this->log_text_buffer,
        &end,
        this->messenger.front().c_str(),
        -1
    );
    this->messenger.pop_front();
    gtk_text_buffer_get_end_iter(this->log_text_buffer, &end);
    gtk_text_view_scroll_to_iter(
        this->log_text_view,
        &end,
        0,
        true,
        0,
        1
    );
}

void GDiskDestroyer::App::launch() {
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
    this->set_log((char*)"");
    this->worker =
        std::thread(
            [=]() {
                try {
                    DiskDestroyer::Writer writer(
                        this->file_name,
                        this->buf_size,
                        this->window,
                        &this->messenger
                    );
                    writer.init();
                    DiskDestroyer::Config::Parser::log(this->pattern_compiled, std::cout);
                    writer((char*)this->pattern_compiled);
                    g_signal_emit_by_name(
                        this->window, "run-success");
                } catch (DiskDestroyer::Gen::Err) {
                    g_signal_emit_by_name(
                        this->window, "run-failure");
                } catch (DiskDestroyer::Writer::Err) {
                    g_signal_emit_by_name(
                        this->window, "run-failure");
                }
            }
        );
    this->worker.detach();
}

void run_button_clicked(GtkButton *button, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->launch();
}

void append_log_handler(GtkWindow *window, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->append_log();
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
    this->set_log((char*)"Success");
}


void run_success_handler(GtkWindow *window, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->run_success();
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
    this->set_log((char*)"Failure");
}

void run_failure_handler(GtkWindow *window, gpointer data) {
    GDiskDestroyer::App *app = (GDiskDestroyer::App*)data;
    app->run_failure();
}

GDiskDestroyer::App::App(int *argc, char ***argv) {
    this->buf_size = 4096;
    this->pattern_compiled = (char*)"";
    this->file_name = "";
    gtk_init(argc, argv);
    this->builder = gtk_builder_new();
    this->build_window((char*)"/com/example/gdide/gdide.glade");
    this->window = GTK_WINDOW(this->get_object((char*)"window"));
    g_signal_connect(this->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
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
    g_signal_new(
        "append-log",
        G_TYPE_OBJECT,
        G_SIGNAL_ACTION,
        0,
        0,
        0,
        0,
        G_TYPE_NONE,
        0
    );
    g_signal_connect(
        window,
        "append-log",
        G_CALLBACK(append_log_handler),
        this
    );
    g_signal_new(
        "run-success",
        G_TYPE_OBJECT,
        G_SIGNAL_ACTION,
        0,
        0,
        0,
        0,
        G_TYPE_NONE,
        0
    );
    g_signal_connect(
        window,
        "run-success",
        G_CALLBACK(run_success_handler),
        this
    );
    g_signal_new(
        "run-failure",
        G_TYPE_OBJECT,
        G_SIGNAL_ACTION,
        0,
        0,
        0,
        0,
        G_TYPE_NONE,
        0
    );
    g_signal_connect(
        window,
        "run-failure",
        G_CALLBACK(run_failure_handler),
        this
    );
    gtk_window_present(this->window);
}

void GDiskDestroyer::App::operator()() {
    gtk_main();
}
