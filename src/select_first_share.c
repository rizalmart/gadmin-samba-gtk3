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
#include "select_first_share.h"

extern char global_share_name[1024];
extern char global_share_comment[1024];
extern char global_share_directory[16384];



void select_first_share(struct w *widgets)
{
    GtkTreePath *path;
    GtkTreeIter iter;
    GtkTreeModel *list_store;
    gchar *share_name = NULL;
    gchar *share_directory = NULL;
    gchar *share_comment = NULL;

    path = gtk_tree_path_new_first();

    list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->share_treeview));

    if( ! gtk_tree_model_get_iter(list_store, &iter, path) )
    {
        gtk_tree_path_free(path);
        strcpy(global_share_name, "");
        return;
    }

    gtk_tree_model_get(list_store, &iter, 0, &share_name, -1);
    gtk_tree_model_get(list_store, &iter, 1, &share_comment, -1);
    gtk_tree_model_get(list_store, &iter, 1, &share_directory, -1);

    snprintf(global_share_name, 1000, "%s", (gchar *) share_name);
    snprintf(global_share_comment, 1000, "%s", (gchar *) share_comment);
    snprintf(global_share_directory, 16000, "%s", (gchar *) share_directory);

    gtk_tree_path_free(path);
    g_free(share_name);
}
