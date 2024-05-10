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
#include "populate_shares.h"
#include "populate_share_settings.h"
#include "select_first_share.h"
#include "show_info.h"
#include "commands.h"
#include "commented.h"
#include "get_option_pos.h"

extern char global_share_name[1024];
extern char global_share_comment[1024];
extern char global_share_directory[16384];



void populate_shares(struct w *widgets)
{
    /* Lists all shares in the sharelist */
    FILE *fp;
    GtkTreeIter iter;
    GtkTreePath *path;
    char *line, *sharepath;
    long conf_size, old_pos = 0, opt_pos;
    long i, share_begin, share_end;
    gboolean edit = 0;
    int found_share = 0;
    gchar *utf8 = NULL;

    gtk_list_store_clear(widgets->share_store);

    /* Populate the sharelist */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        /* Dont show a popup */
        printf("Error opening: [%s] for listing shares\n", SAMBA_CONF);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    sharepath = allocate(conf_size + 1);

    /* Add the shares to the share list */
    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            /* Skip commented lines */
            if( commented(line) )
                continue;

            /* A share is found (not the global settings) */
            if( strstr(line, "[") && strstr(line, "]" )
                && ! strstr(line, "global") && ! commented(line) )
            {
                share_begin = 0;
                share_end = 0;
                for(i = 0; i < conf_size; i++)
                {
                    if( line[i] == '[' )
                    {
                        share_begin = i + 1;
                        break;
                    }
                }

                for(i = i + 1; i < conf_size; i++)
                {
                    if( line[i] == ']' )
                    {
                        share_end = i;
                        break;
                    }
                }

                if( share_end > share_begin )
                {
                    found_share = 1;
                    snprintf(sharepath, share_end, "%s", &line[share_begin]);
                    utf8 = g_locale_to_utf8(sharepath, strlen(sharepath), NULL, NULL, NULL);
                    gtk_list_store_append(GTK_LIST_STORE(widgets->share_store), &iter);
                    gtk_list_store_set(GTK_LIST_STORE(widgets->share_store), &iter, 0, utf8, -1);

                    old_pos = ftell(fp);
                }

                /* Append the share settings */
                if( found_share )
                    while(fgets(line, conf_size, fp) != NULL)
                    {
                        /* Skip commented lines */
                        if( commented(line) )
                            continue;

                        /* If a new share is found, scroll back and break */
                        if( strstr(line, "[") && strstr(line, "]") )
                        {
                            fseek(fp, old_pos, SEEK_SET);
                            break;
                        }

                        /* Path */
                        if( strstr(line, "path") )
                        {
                            opt_pos = get_option_pos(line);
                            snprintf(sharepath, conf_size, "%s", &line[opt_pos]);
                            utf8 =  g_locale_to_utf8(sharepath, strlen(sharepath) - 1, NULL, NULL, NULL);
                            gtk_list_store_set(GTK_LIST_STORE(widgets->share_store), &iter, 1, utf8, -1);
                        }

                        // comment
                        if( strstr(line, "comment") )
                        {
                            opt_pos = get_option_pos(line);
                            snprintf(sharepath, conf_size, "%s", &line[opt_pos]);
                            utf8 =  g_locale_to_utf8(sharepath, strlen(sharepath) - 1, NULL, NULL, NULL);
                            gtk_list_store_set(GTK_LIST_STORE(widgets->share_store), &iter, 2, utf8, -1);
                        }
                    }
            }
        }
    fclose(fp);
    free(line);
    free(sharepath);

    if(utf8 != NULL)
        g_free(utf8);

    /* Cant set treepath if there arent any users/rows, then itll crash */
    /* Also unset global_user_name */
    if( ! found_share )
    {
        strcat(global_share_name, "");
        strcat(global_share_directory, "");
        strcat(global_share_comment, "");
        return;
    }

    path = gtk_tree_path_new_first();
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(widgets->share_treeview), path, NULL, edit);
    gtk_tree_path_free(path);

    select_first_share(widgets);
}
