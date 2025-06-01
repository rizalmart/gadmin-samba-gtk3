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



#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "apply_server_settings.h"

/* Wether or not to let the server settings section expand */
#define EXPAND_SERVER_SETTINGS_SECTION TRUE



void create_server_tab(struct w *widgets)
{
    GtkWidget *settings_treeview_hbox;
    GtkWidget *frame;
    gchar *utf8 = NULL;

    GtkTooltip *tooltips;
    //tooltips = gtk_tooltip_new();



    /* Create the server settings in a table inside a scrolled window */
    //settings_treeview_hbox = gtk_hbox_new(TRUE, 0);
    settings_treeview_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(settings_treeview_hbox),TRUE);
    
	gtk_widget_set_margin_start(widgets->notebook_vbox1, 5);
	gtk_widget_set_margin_end(widgets->notebook_vbox1, 5);
	gtk_widget_set_margin_top(widgets->notebook_vbox1, 5);
	gtk_widget_set_margin_bottom(widgets->notebook_vbox1, 5);    
    
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox1), settings_treeview_hbox,
        EXPAND_SERVER_SETTINGS_SECTION, TRUE, 0);

    widgets->server_settings_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(settings_treeview_hbox), widgets->server_settings_scrolled_window,
        TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widgets->server_settings_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(widgets->server_settings_scrolled_window, -1, 100);

    /* Add a vbox to the scrolled window */
    widgets->server_settings_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(widgets->server_settings_vbox),TRUE);
    
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(widgets->
            server_settings_scrolled_window), widgets->server_settings_vbox);

    /* Add a frame */
    frame = gtk_frame_new(_("Server settings"));

    /* Add a table with for the settings with 3 columns */
    widgets->srv_set_table =
        gtk_table_new(NUM_SERVERTAB_ENTRIES + NUM_SERVERTAB_SPINBUTTONS + NUM_SERVERTAB_COMBOS, 3,
        FALSE);
    gtk_box_pack_start(GTK_BOX(widgets->server_settings_vbox), frame, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame), widgets->srv_set_table);


    /* The Apply server button */
    //GtkWidget *apply_server_button = gtk_button_new_from_stock(GTK_STOCK_APPLY);
    //GtkWidget *server_button_hbox = gtk_hbutton_box_new();
    //gtk_box_pack_start(GTK_BOX(server_button_hbox), apply_server_button, FALSE, FALSE, 0);
    //gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox1), server_button_hbox, FALSE, TRUE, 0);
    //g_signal_connect_swapped(G_OBJECT(apply_server_button), "clicked", 
    //                         G_CALLBACK(apply_server_settings), widgets);


    gtk_widget_show_all(widgets->main_window);

    if(utf8 != NULL)
        g_free(utf8);
}
