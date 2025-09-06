/* GADMIN-SAMBA, an easy to use GTK+ frontend for the SAMBA file and print server.
 * Copyright (C) 2006 - 2012 Magnus Loef <magnus-swe@telia.com> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
*/



#include "../config.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "allocate.h"
#include "gettext.h"
#include "widgets.h"
#include "commands.h"
#include "make_settings_entries.h"
#include "make_settings_combos.h"
#include "make_settings_checkbuttons.h"
#include "reread_conf.h"
#include "load_settings.h"
#include "save_settings.h"
#include "settings_window.h"

#define MAX_CONF_LINE 1000

extern int global_start_winbindd;
extern int global_start_nmbd;


void show_settings_window(struct w *widgets)
{
    GtkWidget *frame;
    GtkWidget *table;
    GtkWidget *settings_vbox;
    GtkTooltip *tooltips;
    gchar *utf8 = NULL;
    gchar *info;

    widgets->settings_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(widgets->settings_window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(widgets->settings_window, -1, -1);

    /* Set window information */
    info = g_strdup_printf(_("GADMIN-SAMBA %s settings"), VERSION);
    gtk_window_set_title(GTK_WINDOW(widgets->settings_window), info);
    g_free(info);

    g_signal_connect(GTK_WINDOW(widgets->settings_window), "delete_event",
        G_CALLBACK(gtk_widget_destroy), NULL);

    settings_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(widgets->settings_window), settings_vbox);


    //tooltips = gtk_tooltip_new();
    frame = gtk_frame_new(_("Settings:"));

    /* A table with 6 settings and 2 columns */
    table = gtk_table_new(7, 2, FALSE);

    gtk_box_pack_start(GTK_BOX(settings_vbox), frame, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame), table);


    /* Max length and input 350 chars */

    /* Show standard conf question */
    widgets->settings_checkbutton[0] =
        make_checkbutton_with_label(GTK_TABLE(table), _(" New samba configuration: "), 0, 1, 1, 2);

    /* Start netbios checkbutton */
    widgets->settings_checkbutton[2] =
        make_checkbutton_with_label(GTK_TABLE(table), _(" Start netbios server: "), 0, 1, 2, 3);

    /* Start winbindd checkbutton */
    widgets->settings_checkbutton[1] =
        make_checkbutton_with_label(GTK_TABLE(table), _(" Start winbind server: "), 0, 1, 3, 4);

    if( global_start_winbindd )
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[1]), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[1]), FALSE);

    if( global_start_nmbd )
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[2]), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[2]), FALSE);

    /* Path to smb.conf */
    widgets->settings_entry[0] =
        make_entry_with_label(GTK_TABLE(table), _(" Path to smb.conf: "), 0, 1, 4, 5, 350);
    utf8 = g_locale_to_utf8(SAMBA_CONF, strlen(SAMBA_CONF), NULL, NULL, NULL);
    gtk_entry_set_text(GTK_ENTRY(widgets->settings_entry[0]), utf8);


    GtkWidget *settings_hbuttonbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(settings_vbox), settings_hbuttonbox, FALSE, FALSE, 0);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(settings_hbuttonbox), GTK_BUTTONBOX_SPREAD);

    //GtkWidget *apply_settings_button = gtk_button_new_from_stock(GTK_STOCK_APPLY);
    
    GtkWidget *apply_settings_button = gtk_button_new_with_mnemonic("_Apply");
	GtkWidget *apply_settings_icon = gtk_image_new_from_icon_name("gtk-apply", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(apply_settings_button), apply_settings_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(apply_settings_button), TRUE);

    gtk_container_add(GTK_CONTAINER(settings_hbuttonbox), apply_settings_button);


    g_signal_connect_swapped(G_OBJECT(apply_settings_button), "clicked",
        G_CALLBACK(apply_settings_button_clicked), widgets);

    GtkWidget *cancel_settings_button = gtk_button_new_with_mnemonic("_Cancel");
	GtkWidget *cancel_settings_icon = gtk_image_new_from_icon_name("gtk-cancel", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(cancel_settings_button), cancel_settings_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(cancel_settings_button), TRUE);    
    
    
    gtk_container_add(GTK_CONTAINER(settings_hbuttonbox), cancel_settings_button);
    g_signal_connect_swapped(G_OBJECT(cancel_settings_button), "clicked",
        G_CALLBACK(cancel_settings_button_clicked), widgets);

    if(utf8 != NULL)
        g_free(utf8);

    gtk_widget_show_all(widgets->settings_window);
}


void cancel_settings_button_clicked(struct w *widgets)
{
    gtk_widget_destroy(widgets->settings_window);
}


void apply_settings_button_clicked(struct w *widgets)
{
    /* The apply button in the settings window */

    save_settings(widgets);

    load_settings(widgets);

    gtk_widget_destroy(widgets->settings_window);
}
