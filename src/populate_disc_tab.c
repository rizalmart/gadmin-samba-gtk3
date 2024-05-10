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
#include "support.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "allocate.h"
#include "widgets.h"
#include "populate_disc_tab.h"
#include "show_info.h"


#define MAX_READ_POPEN 16384

#define DISC_USAGE "df -hP | grep /"


void populate_discs(struct w *widgets)
{
    /* Lists discs in the disc treeview */
    FILE *fp;
    GtkTreeIter iter;
    GtkTreePath *path;
    gboolean edit = 0;
    int found = 0;
    gchar *utf8 = NULL;

    char buf[MAX_READ_POPEN] = "";
    char mount[8192] = "", free[1024] = "", used[1024] = "", total[1024] = "", percent[1024] =
        "", device[8192] = "";

    gtk_list_store_clear(widgets->disc_store);


    if((fp = popen(DISC_USAGE, "r")) == NULL)
    {
        printf("Error running command %s\n", DISC_USAGE);
        return;
    }
    fflush(fp);

    while(fgets(buf, MAX_READ_POPEN, fp) != NULL)
    {
        sscanf(buf, "%s %s %s %s %s %s", device, total, used, free, percent, mount);

        gtk_list_store_append(GTK_LIST_STORE(widgets->disc_store), &iter);

        utf8 = g_locale_to_utf8(mount, strlen(mount), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->disc_store), &iter, 0, utf8, -1);

        utf8 = g_locale_to_utf8(free, strlen(free), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->disc_store), &iter, 1, utf8, -1);

        utf8 = g_locale_to_utf8(used, strlen(used), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->disc_store), &iter, 2, utf8, -1);

        utf8 = g_locale_to_utf8(total, strlen(total), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->disc_store), &iter, 3, utf8, -1);

        utf8 = g_locale_to_utf8(percent, strlen(percent), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->disc_store), &iter, 4, utf8, -1);

        utf8 = g_locale_to_utf8(device, strlen(device), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->disc_store), &iter, 5, utf8, -1);

        found = 1;
    }
    pclose(fp);

    if(utf8 != NULL)
        g_free(utf8);

    /* Cant set treepath if there arent any discs/rows, then itll crash */
    if( ! found )
        return;

    path = gtk_tree_path_new_first();
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(widgets->disc_treeview), path, NULL, edit);
    gtk_tree_path_free(path);
}
