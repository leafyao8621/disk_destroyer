#include <iostream>
#include <sstream>

#include "../backend/config/config.h"
#include "../backend/config/parser.h"
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

void builtin_combo_box_changed(GtkComboBox *combo_box, gpointer *data) {
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

GDiskDestroyer::App::App(int *argc, char ***argv) {
    ready = false;
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
    this->log_text_buffer =
        gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(
                this->get_object((char*)"log_text_view")
            )
        );
}

void GDiskDestroyer::App::init() {
    GtkListStore *list =
        GTK_LIST_STORE(this->get_object((char*)"rc_pattern_list_store"));
    GtkTreeIter iter;
    for (auto &i : this->rc_config) {
        gtk_list_store_append(list, &iter);
        gtk_list_store_set(list, &iter, 0, i.first.c_str(), -1);
    }
    g_signal_connect(
        built_in_radio_button,
        "toggled",
        G_CALLBACK(built_in_radio_button_toggled),
        this
    );
    g_signal_connect(
        rc_radio_button,
        "toggled",
        G_CALLBACK(rc_radio_button_toggled),
        this
    );
    g_signal_connect(
        custom_radio_button,
        "toggled",
        G_CALLBACK(custom_radio_button_toggled),
        this
    );
    g_signal_connect(
        built_in_combo_box,
        "changed",
        G_CALLBACK(builtin_combo_box_changed),
        this
    );
    gtk_window_present(this->window);
}

void GDiskDestroyer::App::operator()() {
    gtk_main();
}
