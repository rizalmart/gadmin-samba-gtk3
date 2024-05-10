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
#include "create_macct_tab.h"
#include "macct_treeview_row_clicked.h"

#define EXPAND_MACCT_SECTION FALSE
#define EXPAND_MACCT_SETTINGS_SECTION TRUE



void create_macct_tab(struct w *widgets)
{
    GtkCellRenderer *macct_cell_renderer;
    GtkWidget *macct_treeview_hbox, *macct_settings_treeview_hbox;
    GtkWidget *macct_scrolled_window;
    GtkWidget *frame;
    gchar *utf8 = NULL;

    GtkTooltip *tooltips;
    //tooltips = gtk_tooltip_new();


    /* Create the machine account list treeview in a scrolled window */
    macct_treeview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox4), macct_treeview_hbox, EXPAND_MACCT_SECTION, TRUE, 0);

    macct_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(macct_treeview_hbox), macct_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(macct_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(macct_scrolled_window, -1, 150);

    widgets->macct_store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    widgets->macct_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->macct_treeview),
        GTK_TREE_MODEL(widgets->macct_store));

    gtk_container_add(GTK_CONTAINER(macct_scrolled_window), widgets->macct_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->macct_treeview), TRUE);

    /* Set the column labels in the treeview */
    macct_cell_renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *macct_col =
        gtk_tree_view_column_new_with_attributes(_("NetBIOS machine name"), macct_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->macct_treeview),
        GTK_TREE_VIEW_COLUMN(macct_col));


    GtkTreeViewColumn *group_col =
        gtk_tree_view_column_new_with_attributes(_("Machine group"), macct_cell_renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->macct_treeview),
        GTK_TREE_VIEW_COLUMN(group_col));


    GtkTreeViewColumn *comment_col =
        gtk_tree_view_column_new_with_attributes(_("Machine comment"), macct_cell_renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->macct_treeview),
        GTK_TREE_VIEW_COLUMN(comment_col));


    g_signal_connect((gpointer) widgets->macct_treeview, "button_press_event",
        G_CALLBACK(macct_treeview_row_clicked), widgets);


    /* Create the machine account settings scrolled window with a frame and a table */
    macct_settings_treeview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox4), macct_settings_treeview_hbox,
        EXPAND_MACCT_SETTINGS_SECTION, TRUE, 0);

    widgets->macct_settings_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(macct_settings_treeview_hbox),
        widgets->macct_settings_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widgets->macct_settings_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(widgets->macct_settings_scrolled_window, -1, 100);

    /* Add a vbox to the scrolled window */
    widgets->macct_settings_vbox = gtk_vbox_new(FALSE, 0);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(widgets->
            macct_settings_scrolled_window), widgets->macct_settings_vbox);

    /* Add a frame */
    frame = gtk_frame_new(_("Machine account settings:"));

    /* A table with NUM settings and 2 columns FIXME, Count */

//FIXME.. wrong count
    widgets->macct_set_table = gtk_table_new(NUM_MACCTTAB_ENTRIES, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(widgets->macct_settings_vbox), frame, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame), widgets->macct_set_table);

    gtk_widget_show_all(widgets->main_window);

    if( utf8 != NULL )
        g_free(utf8);
}
