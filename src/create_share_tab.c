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
#include "functions.h"
#include "create_share_tab.h"
#include "share_treeview_row_clicked.h"


#define EXPAND_SHARE_SECTION FALSE
#define EXPAND_SHARE_SETTINGS_SECTION TRUE



void create_share_tab(struct w *widgets)
{
    GtkCellRenderer *share_cell_renderer;
    GtkWidget *share_treeview_hbox, *share_settings_treeview_hbox;
    GtkWidget *share_scrolled_window;
    GtkWidget *frame;
    gchar *utf8 = NULL;

    GtkTooltip *tooltips;
    //tooltips = gtk_tooltip_new();

    /* Create the sharelist treeview in a scrolled window */
    share_treeview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox3), share_treeview_hbox, EXPAND_SHARE_SECTION, TRUE, 0);

    share_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(share_treeview_hbox), share_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(share_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(share_scrolled_window, -1, 100);

    widgets->share_store =
        gtk_list_store_new(15, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    widgets->share_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->share_treeview),
        GTK_TREE_MODEL(widgets->share_store));
    gtk_container_add(GTK_CONTAINER(share_scrolled_window), widgets->share_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->share_treeview), TRUE);

    /* Set the column labels in the treeview */
    share_cell_renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *share_col =
        gtk_tree_view_column_new_with_attributes(_("Share name"), share_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->share_treeview),
        GTK_TREE_VIEW_COLUMN(share_col));

    GtkTreeViewColumn *dir_col =
        gtk_tree_view_column_new_with_attributes(_("Directory"), share_cell_renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->share_treeview),
        GTK_TREE_VIEW_COLUMN(dir_col));

    GtkTreeViewColumn *comment_col =
        gtk_tree_view_column_new_with_attributes(_("Comment"), share_cell_renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->share_treeview),
        GTK_TREE_VIEW_COLUMN(comment_col));

    g_signal_connect((gpointer) widgets->share_treeview, "button_press_event",
        G_CALLBACK(share_treeview_row_clicked), widgets);

    /* Create the share settings scrolled window with a frame and a table */
    share_settings_treeview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox3), share_settings_treeview_hbox,
        EXPAND_SHARE_SETTINGS_SECTION, TRUE, 0);

    widgets->share_settings_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(share_settings_treeview_hbox),
        widgets->share_settings_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widgets->share_settings_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(widgets->share_settings_scrolled_window, -1, 100);

    /* Add a vbox to the scrolled window */
    widgets->share_settings_vbox = gtk_vbox_new(FALSE, 0);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(widgets->
            share_settings_scrolled_window), widgets->share_settings_vbox);

    /* Add a frame */
    frame = gtk_frame_new(_("Share settings:"));

    /* A table with NUM settings and 2 columns FIXME, Count */
    widgets->share_set_table =
        gtk_table_new(NUM_SHARETAB_ENTRIES + NUM_SHARETAB_SPINBUTTONS + NUM_SHARETAB_CHECKBUTTONS, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(widgets->share_settings_vbox), frame, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame), widgets->share_set_table);


    gtk_widget_show_all(widgets->main_window);

    if(utf8 != NULL)
        g_free(utf8);
}
