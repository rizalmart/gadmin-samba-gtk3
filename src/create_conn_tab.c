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
#include <unistd.h>
#include "widgets.h"
#include "gettext.h"
#include "create_conn_tab.h"
#include "populate_conns.h"
#include "commands.h"


/* Wether or not to let the connection section expand */
#define EXPAND_CONN_SECTION TRUE

void disconnect_conn(struct w *widgets);

void kill_pid(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, struct w *widgets);



void disconnect_conn(struct w *widgets)
{
    GtkTreeSelection *selection;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->conns_treeview));

    gtk_tree_selection_selected_foreach(GTK_TREE_SELECTION(selection),
        (GtkTreeSelectionForeachFunc) & kill_pid, widgets);

    /* We need to sleep some before we can update the gui */
    sleep(1);
    populate_conns(widgets);
}



/* The contents of the selections in the list is iterated and the selected PIDs are killed */
void kill_pid(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, struct w *widgets)
{
    gchar *pid, *cmd;
    gtk_tree_model_get(model, iter, 0, &pid, -1);

    /* SIGTERM the PID */
    cmd = g_strdup_printf("kill -15 %s", pid);
    if( ! run_command(cmd) )
    {
        printf("Error killing PID: %s\n", pid);
    }
    g_free(cmd);
    g_free(pid);
}



void create_conn_tab(struct w *widgets)
{
    GtkCellRenderer *conn_cell_renderer;
    GtkWidget *conn_treeview_hbox;
    GtkWidget *conn_scrolled_window;
    GtkTreeSelection *selection;
    gchar *utf8 = NULL;

    GtkTooltip *tooltips;
    //tooltips = gtk_tooltip_new();

    /* Create the conn treeview in a scrolled window */
    conn_treeview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox6), conn_treeview_hbox, EXPAND_CONN_SECTION, TRUE, 0);

    conn_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(conn_treeview_hbox), conn_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(conn_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    widgets->conns_store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                               G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    widgets->conns_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->conns_treeview),
        GTK_TREE_MODEL(widgets->conns_store));

    /* Selection is multiple */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->conns_treeview));
    gtk_tree_selection_set_mode(GTK_TREE_SELECTION(selection), GTK_SELECTION_MULTIPLE);

    gtk_container_add(GTK_CONTAINER(conn_scrolled_window), widgets->conns_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->conns_treeview), TRUE);


    /* Set the column labels in the treeview */
    conn_cell_renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *pid_col =
        gtk_tree_view_column_new_with_attributes(_("Process ID"), conn_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->conns_treeview),
        GTK_TREE_VIEW_COLUMN(pid_col));

    GtkTreeViewColumn *user_col =
        gtk_tree_view_column_new_with_attributes(_("User"), conn_cell_renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->conns_treeview),
        GTK_TREE_VIEW_COLUMN(user_col));

    GtkTreeViewColumn *group_col =
        gtk_tree_view_column_new_with_attributes(_("Group"), conn_cell_renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->conns_treeview),
        GTK_TREE_VIEW_COLUMN(group_col));

    GtkTreeViewColumn *server_col =
        gtk_tree_view_column_new_with_attributes(_("Client computer"), conn_cell_renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->conns_treeview),
        GTK_TREE_VIEW_COLUMN(server_col));

    GtkTreeViewColumn *ip_col =
        gtk_tree_view_column_new_with_attributes(_("Client address"), conn_cell_renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->conns_treeview),
        GTK_TREE_VIEW_COLUMN(ip_col));

    GtkTreeViewColumn *file_col =
        gtk_tree_view_column_new_with_attributes(_("File in use"), conn_cell_renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->conns_treeview),
        GTK_TREE_VIEW_COLUMN(file_col));


    /* The update conn button */
    GtkWidget *conn_button_box = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox6), conn_button_box, FALSE, FALSE, 0);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(conn_button_box), GTK_BUTTONBOX_SPREAD);

    GtkWidget *dc_conn_button = gtk_button_new_from_stock(GTK_STOCK_DELETE);
    gtk_box_pack_start(GTK_BOX(conn_button_box), dc_conn_button, FALSE, FALSE, 0);
    //gtk_tooltips_set_tip(tooltips, dc_conn_button, _("Disconnect selected processes"), NULL);
    gtk_widget_set_tooltip_text(dc_conn_button, _("Disconnect selected processes"));    
    

    GtkWidget *update_conn_button = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
    gtk_box_pack_start(GTK_BOX(conn_button_box), update_conn_button, FALSE, FALSE, 0);


    g_signal_connect_swapped(G_OBJECT(update_conn_button), "clicked",
        G_CALLBACK(populate_conns), widgets);

    g_signal_connect_swapped(G_OBJECT(dc_conn_button), "clicked",
        G_CALLBACK(disconnect_conn), widgets);

    gtk_widget_show_all(widgets->main_window);

    if( utf8 != NULL )
        g_free(utf8);
}
