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
#include "populate_macct_settings.h"

extern char global_macct_name[1024];



void macct_treeview_row_clicked(GtkTreeView * treeview, GdkEventButton * event, gpointer data)
{
    gchar *macct_name;
    GtkTreeIter iter;
    GtkTreePath *path;
    GtkTreeModel *list_store;
    GtkTreeSelection *selection;

    /* The left button is pressed */
    if( ! event->type == GDK_BUTTON_PRESS || ! event->button == 1 )
        return;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if( gtk_tree_selection_count_selected_rows(selection) <= 1 )
    {
        /* Get the treepath for the row that was clicked */
        if( gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y,
                &path, NULL, NULL, NULL) )
        {
            gtk_tree_selection_unselect_all(selection);
            gtk_tree_selection_select_path(selection, path);

            list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
            gtk_tree_model_get_iter(list_store, &iter, path);
            gtk_tree_model_get(list_store, &iter, 0, &macct_name, -1);

            sprintf(global_macct_name, "%s$", (gchar *) macct_name);

            g_free(macct_name);
            gtk_tree_path_free(path);

            populate_macct_settings(data);
        }
    }
}
