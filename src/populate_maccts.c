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
#include "functions.h"
#include "populate_maccts.h"
#include "show_info.h"
#include "commands.h"

extern char global_macct_name[1024];



void populate_maccts(struct w *widgets)
{
    /* Lists all the samba machine accounts in the macine account list */
    FILE *fp;
    GtkTreeIter iter;
    GtkTreePath *path;
    char *line, *username;
    long conf_size;
    gboolean edit = 0;
    int i, found_user = 0;
    gchar *utf8 = NULL;
    gchar *what, *val, *groupname;

    gtk_list_store_clear(widgets->macct_store);

    /* Puts the pdbedit userlist as:
       /etc/gadmin-samba/gadmin_samba_users */
    get_gadmin_samba_users();

    /* Populate the machine account list */
    if((fp = fopen(GADMIN_SAMBA_USERS, "r")) == NULL)
    {
        /* Dont show a popup */
        printf("Error opening: [%s] for listing machine accounts\n", GADMIN_SAMBA_USERS);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    username = allocate(conf_size + 1);

    /* Add the machine accounts to the list */
    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            /* Skip commented lines, skip regular user accounts */
            if(strstr(line, "#") || !strstr(line, "$") || strlen(line) < 10)
                continue;

            for(i = 0; i < conf_size - 1; i++)
            {
                if( line[i] == ':' )
                {
                    found_user = 1;

                    /* Add a row and insert the machine account (minus '$') in the list */
                    snprintf(username, i + 1, "%s", line);
                    utf8 = g_locale_to_utf8(username, strlen(username) - 1, NULL, NULL, NULL);
                    gtk_list_store_append(GTK_LIST_STORE(widgets->macct_store), &iter);
                    gtk_list_store_set(GTK_LIST_STORE(widgets->macct_store), &iter, 0, utf8, -1);

                    /* Get the group number for this user then translate it into 
                       the group name and insert it in the list */
                    what = g_strdup_printf("group");
                    val = get_user_setting(username, what);
                    groupname = get_group_name(val);
                    utf8 = g_locale_to_utf8(groupname, strlen(groupname), NULL, NULL, NULL);
                    gtk_list_store_set(GTK_LIST_STORE(widgets->macct_store), &iter, 1, utf8, -1);
                    g_free(what);
                    g_free(val);
                    g_free(groupname);

                    /* Get the users comment and insert it in the list */
                    what = g_strdup_printf("comment");
                    val = get_user_setting(username, what);
                    utf8 = g_locale_to_utf8(val, strlen(val), NULL, NULL, NULL);
                    gtk_list_store_set(GTK_LIST_STORE(widgets->macct_store), &iter, 2, utf8, -1);
                    g_free(what);
                    g_free(val);

                    break;
                }
            }
        }
    fclose(fp);
    free(line);
    free(username);

    if(utf8 != NULL)
        g_free(utf8);

    if( ! found_user )
    {
        strcat(global_macct_name, "");
        return;
    }
//    else
//      {
    /* Set the name and comment in the machine account entries */
//    gtk_entry_set_text(GTK_ENTRY(widgets->macct_set_entry[0]), global_macct_name);      

//    what = g_strdup_printf("comment");
//        val = get_user_setting((gchar *)global_macct_name, what);
//    gtk_entry_set_text(GTK_ENTRY(widgets->macct_set_entry[1]), val);      
//    g_free(what); g_free(val);
//      }

    path = gtk_tree_path_new_first();
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(widgets->macct_treeview), path, NULL, edit);
    gtk_tree_path_free(path);
}
