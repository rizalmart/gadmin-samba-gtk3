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
#include "select_first_user.h"

extern char global_user_name[1024];



void select_first_user(struct w *widgets)
{
    GtkTreePath *path;
    GtkTreeIter iter;
    GtkTreeModel *list_store;
    gchar *user = NULL;

    path = gtk_tree_path_new_first();

    list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->user_treeview));

    if( ! gtk_tree_model_get_iter(list_store, &iter, path) )
    {
        gtk_tree_path_free(path);
        strcpy(global_user_name, "");
        return;
    }

    gtk_tree_model_get(list_store, &iter, 0, &user, -1);

    snprintf(global_user_name, 1000, "%s", (gchar *) user);

    gtk_tree_path_free(path);
    g_free(user);
}
