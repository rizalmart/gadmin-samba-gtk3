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
#include "populate_conns.h"
#include "show_info.h"
#include "commands.h"



void populate_conns(struct w *widgets)
{
    /* Lists connections to the samba server and its shares */
    FILE *fp;
    GtkTreeIter iter;
    GtkTreePath *path;
    char *line, *input1, *input2, *input3, *input4, *input5, *input6, *tmp_pid;
    gboolean edit = 0;
    int found_conn = 0;
    int i = 0, z = 0, delim = 0, space = 0;
    long file_size = 0, old_pos = 0;
    gchar *combined = NULL;
    gchar *utf8 = NULL;

    char *cmd_buf;
    char filename[] = "/tmp/gadmin-samba-XXXXXX";
    int tmpfd = -1;

    gtk_list_store_clear(widgets->conns_store);

    if(!file_exists("/tmp"))
    {
        printf("The directory: /tmp doesnt exist, returning.\n");
        return;
    }

    /* Flush stdin and stdout */
    fflush(stdin);
    fflush(stdout);

    /* Create a temp file to hold our smbstatus output */
    tmpfd = mkstemp(filename);
    if( tmpfd < 0 )
    {
        printf("Could not create temporary file: %s\n", filename);
        return;
    }
    close(tmpfd);

    /* Write the output to the temp file */
    cmd_buf = allocate(4096);
    snprintf(cmd_buf, 4000, "smbstatus 2>&1 > %s", filename);
    if((fp = popen(cmd_buf, "w")) == NULL)
    {
        if(strstr(filename, "/tmp/gadmin-samba"))
            unlink(filename);

        /* Dont show a popup */
        free(cmd_buf);
        return;
    }
    pclose(fp);
    free(cmd_buf);

    /* We want the output in a tempfile so we can scroll around in it properly */
    if((fp = fopen((char *)filename, "r")) == NULL)
    {
        if( strstr(filename, "/tmp/gadmin-samba") )
            unlink(filename);

        /* Dont show a popup */
        return;
    }
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    if( file_size < 2 )
    {
        if(strstr(filename, "/tmp/gadmin-samba"))
            unlink(filename);

        fclose(fp);
        return;
    }

    line = allocate(file_size + 1);
    input1 = allocate(4096);
    input2 = allocate(4096);
    input3 = allocate(4096);
    input4 = allocate(4096);

    /* For the filepaths */
    input5 = allocate(16384);
    tmp_pid = allocate(16384);

/*
  PID    User       Group         Machine       (IP = doesnt show)    (Share, pop downwards with PID to PID matching)
  -------------------------------------------------------------------
  7324   mange      mange         samba24       (127.0.0.1)  	      /home/foo/glibc...
  7326   mange      mange         samba24       (127.0.0.1)  	      /var/cad/dwg.dwg
  

  Service ........... connected at      etc    etc
  -------------------------------------------------------------------


  Locked files:
  Pid          DenyMode   Access      R/W        Oplock           SharePath           Name
  ----------------------------------------------------------------------------------------
  7324         DENY_NONE  0x120089    RDONLY     NONE             /home   mange/GLIBC/glibc-2.3.6.tar.bz2   Sun Jun 25 23:34:59 2006
  7326         DENY_NONE  0x120089    RDONLY     NONE             /home   mange/GLIBC/glibc-2.3.6.tar.bz2   Sun Jun 25 23:35:17 2006
*/


    /* Scroll to the first divider line "------" */
    while(fgets(line, file_size, fp) != NULL)
        if( strstr(line, "---") )
            break;

    while(fgets(line, file_size, fp) != NULL)
    {
        /* Skip stuff we dont want to list */
        if( strlen(line) < 10 || strlen(line) > 4000 )
            continue;

        if( strstr(line, "---") || strstr(line, "Connected at") )
            break;

        /* Grab row output */
        sscanf(line, "%s %s %s %s %s", input1, input2, input3, input4, input5);

        /* Add a row */
        gtk_list_store_append(GTK_LIST_STORE(widgets->conns_store), &iter);

        /* Insert PID */
        utf8 = g_locale_to_utf8(input1, strlen(input1), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->conns_store), &iter, 0, utf8, -1);

        /* Insert User name */
        utf8 = g_locale_to_utf8(input2, strlen(input2), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->conns_store), &iter, 1, utf8, -1);

        /* Insert Group name */
        utf8 = g_locale_to_utf8(input3, strlen(input3), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->conns_store), &iter, 2, utf8, -1);

        /* Insert Server name */
        utf8 = g_locale_to_utf8(input4, strlen(input4), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->conns_store), &iter, 3, utf8, -1);

        /* Insert IP-Address */
        utf8 = g_locale_to_utf8(input5, strlen(input5), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->conns_store), &iter, 4, utf8, -1);

        found_conn = 1;

        if( strstr(line, "---") )
            break;

        /* Record the old position */
        old_pos = ftell(fp);

        /* Scroll to the second divider line "------" */
        while(fgets(line, file_size, fp) != NULL)
            if( strstr(line, "---") )
                break;

        /* Scroll to the third divider line "------" */
        while(fgets(line, file_size, fp) != NULL)
            if( strstr(line, "---") )
                break;

        /* List the files on the corresponding PID line */
        while(fgets(line, file_size, fp) != NULL)
        {
            /* Skip stuff we dont want to list */
            if( strlen(line) < 10 || strlen(line) > 16000 )
                continue;

            /* Grab row output (PID, /sharepath, dir/file) */
            sscanf(line, "%s %*s %*s %*s %*s %s", tmp_pid, input5);

            /* Handle Split filenames (grab from the first '/' to the end) */
            delim = 0;
            for(i = 0; line[i] != '\0'; i++)
            {
                /* Scroll to the second '/', the first share path is collected above */
                if(line[i] == '/')
                    delim++;

                if(delim == 2)
                    break;
            }

            /* Delimiter '/' found twice and its the correct PID, insert the line */
            if( delim == 2 && found_conn && ! strcmp(tmp_pid, input1) )
            {
                /* The line also contains the date at the end, skip it */
                space = 0;
                for(z = strlen(line) - 1; z > 0; z--)
                {
                    if( line[z] != ' ' && line[z + 1] == ' ' )
                        space++;

                    if( space == 5 )
                        break;
                }
                z = z + 2;

                if( z - 1 < 16000 )
                {
                    input6 = allocate(16384);
                    snprintf(input6, z - i, "%s", &line[i]);

                    /* Snip at the end */
                    input6[z] = '\0';

                    combined = g_strdup_printf("%s%s", input5, input6);
                    utf8 = g_locale_to_utf8(combined, strlen(combined), NULL, NULL, NULL);
                    gtk_list_store_set(GTK_LIST_STORE(widgets->conns_store), &iter, 5, utf8, -1);
                    g_free(combined);
                    free(input6);
                }
                break;
            }
        }

        found_conn = 0;

        /* Scroll to the old position */
        fseek(fp, old_pos, SEEK_SET);
    }
    fclose(fp);

    /* Remove temporary file */
    if( strstr(filename, "/tmp/gadmin-samba") )
        unlink(filename);

    free(line);
    free(input1);
    free(input2);
    free(input3);
    free(input4);
    free(input5);
    free(tmp_pid);

    if( utf8 != NULL )
        g_free(utf8);

    /* Cant set path if there are no rows */
    if( ! found_conn )
        return;

    path = gtk_tree_path_new_first();
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(widgets->conns_treeview), path, NULL, edit);
    gtk_tree_path_free(path);
}
