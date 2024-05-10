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
#include "create_user_tab.h"
#include "user_treeview_row_clicked.h"

#define EXPAND_USER_SECTION FALSE
#define EXPAND_USER_SETTINGS_SECTION TRUE



void create_user_tab(struct w *widgets)
{
    GtkCellRenderer *user_cell_renderer;
    GtkWidget *user_treeview_hbox, *user_settings_treeview_hbox;
    GtkWidget *user_scrolled_window;
    GtkWidget *frame;
    gchar *utf8 = NULL;

    GtkTooltip *tooltips;
    //tooltips = gtk_tooltip_new();


    /* Create the userlist treeview in a scrolled window */
    user_treeview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox2), user_treeview_hbox, EXPAND_USER_SECTION,
        TRUE, 0);

    user_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(user_treeview_hbox), user_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(user_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(user_scrolled_window, -1, 100);

    widgets->user_store =
        gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    widgets->user_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->user_treeview),
        GTK_TREE_MODEL(widgets->user_store));

    gtk_container_add(GTK_CONTAINER(user_scrolled_window), widgets->user_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->user_treeview), TRUE);

    /* Set the column labels in the treeview */
    user_cell_renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *user_col =
        gtk_tree_view_column_new_with_attributes(_("User"), user_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->user_treeview),
        GTK_TREE_VIEW_COLUMN(user_col));

    GtkTreeViewColumn *group_col =
        gtk_tree_view_column_new_with_attributes(_("Group"), user_cell_renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->user_treeview),
        GTK_TREE_VIEW_COLUMN(group_col));

    GtkTreeViewColumn *homedir_col =
        gtk_tree_view_column_new_with_attributes(_("Home directory"), user_cell_renderer, "text", 2,
        NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->user_treeview),
        GTK_TREE_VIEW_COLUMN(homedir_col));

    GtkTreeViewColumn *comment_col =
        gtk_tree_view_column_new_with_attributes(_("Comment"), user_cell_renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->user_treeview),
        GTK_TREE_VIEW_COLUMN(comment_col));


    g_signal_connect((gpointer) widgets->user_treeview, "button_press_event",
        G_CALLBACK(user_treeview_row_clicked), widgets);


    /* Create the user settings scrolled window with a frame and a table */
    user_settings_treeview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox2), user_settings_treeview_hbox,
        EXPAND_USER_SETTINGS_SECTION, TRUE, 0);

    widgets->user_settings_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(user_settings_treeview_hbox), widgets->user_settings_scrolled_window,
        TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widgets->user_settings_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(widgets->user_settings_scrolled_window, -1, 100);

    /* Add a vbox to the scrolled window */
    widgets->user_settings_vbox = gtk_vbox_new(FALSE, 0);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(widgets->
            user_settings_scrolled_window), widgets->user_settings_vbox);

    /* Add a frame */
    frame = gtk_frame_new(_("User settings:"));

    /* A table with NUM settings and 2 columns FIXME, Count */
    widgets->user_set_table =
        gtk_table_new(NUM_USERTAB_ENTRIES + NUM_USERTAB_SPINBUTTONS + NUM_USERTAB_CHECKBUTTONS, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(widgets->user_settings_vbox), frame, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame), widgets->user_set_table);


    gtk_widget_show_all(widgets->main_window);

    if(utf8 != NULL)
        g_free(utf8);
}
