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
#include <dirent.h>
#include "chars_are_digits.h"
#include "commands.h"
#include "status_update.h"
#include "show_info.h"

extern int activated;
extern int global_smbd_activated;
extern int global_nmbd_activated;
extern int global_winbindd_activated;

extern int MAX_READ_POPEN;


char * get_process_pid(char process[1024])
{
    FILE *fp;
    long num, file_size = 4000;
    char *line, *sub_proc_path, *pid;
    int x = 0;
    struct dirent **namelist;

    pid = allocate(1024);
    strcpy(pid, "NONE");

    sub_proc_path = allocate(1024);

    num = scandir(PROC_PATH, &namelist, 0, alphasort);
    if( num < 0 )
    {
        perror("scandir");
        return pid;
    }
    else
    {
        /* List all directories in PROC_PATH */
        for(x = 0; x < num; x++)
        {
            /* Now list PROC_PATH/24207/cmdline */
            snprintf(sub_proc_path, 1000, "%s/%s/cmdline", PROC_PATH, namelist[x]->d_name);

            if((fp = fopen(sub_proc_path, "r")) == NULL)
            {
                free(namelist[x]);
                continue;
            }
            line = allocate(file_size + 1);

            if( file_size > 1 )
                while(fgets(line, file_size, fp) != NULL)
                {
                    /* If the following string is in this file its running */
                    if( strstr(line, process) )
                    {
                        snprintf(pid, 1000, "%s", namelist[x]->d_name);
                        break;
                    }
                }
            fclose(fp);
            free(line);
            free(namelist[x]);
        }
    }
    free(namelist);
    free(sub_proc_path);

    return pid;
}


int status_update(struct w *widgets)
{
    /* Activation status for smbd, nmbd and winbindd. */
    gchar *info, *utf8 = NULL;
    char *pid1, *pid2, *pid3, *ext_info;
    GdkColor color;

    pid1 = get_process_pid("smbd");
    pid2 = get_process_pid("nmbd");
    pid3 = get_process_pid("winbindd");

    /* First set activation status globally (used when deactivating) */
    if( strstr(pid1, "NONE") )
        global_smbd_activated = 0;
    else
        global_smbd_activated = 1;

    if( strstr(pid2, "NONE") )
        global_nmbd_activated = 0;
    else
        global_nmbd_activated = 1;

    if( strstr(pid3, "NONE") )
        global_winbindd_activated = 0;
    else
        global_winbindd_activated = 1;


    /* If no daemons are running its deactivated */
    if( ! global_smbd_activated && ! global_nmbd_activated && ! global_winbindd_activated )
    {
        activated = 0;
        info = g_strdup_printf(_("Status: Deactivated"));
        utf8 = g_locale_to_utf8(info, strlen(info), NULL, NULL, NULL);
        gtk_label_set_text(GTK_LABEL(widgets->status_label), utf8);
        g_free(info);
        free(pid1);
        free(pid2);
        free(pid3);

        /* Set status color */
        gdk_color_parse("red", &color);
        gtk_widget_modify_fg(widgets->status_label, GTK_STATE_NORMAL, &color);

        return (TRUE);
    }
    else
    {
        /* Atleast one of the daemons are running */
        activated = 1;

        if( ! global_smbd_activated || ! global_nmbd_activated || ! global_winbindd_activated )
        {
            ext_info = allocate(1024);
            strcpy(ext_info, ", inactive servers:");

            if( ! global_smbd_activated )
                strcat(ext_info, " smbd ");

            if( ! global_nmbd_activated )
                strcat(ext_info, " nmbd ");

            if( ! global_winbindd_activated )
                strcat(ext_info, " winbindd ");

            info = g_strdup_printf(_("Status: Activated %s"), ext_info);
            free(ext_info);

            /* Set status color */
            gdk_color_parse("dark green", &color);
            gtk_widget_modify_fg(widgets->status_label, GTK_STATE_NORMAL, &color);
        }
        else
        {
            info = g_strdup_printf(_("Status: Activated"));

            /* Set status color */
            gdk_color_parse("dark green", &color);
            gtk_widget_modify_fg(widgets->status_label, GTK_STATE_NORMAL, &color);
        }
        utf8 = g_locale_to_utf8(info, strlen(info), NULL, NULL, NULL);
        gtk_label_set_text(GTK_LABEL(widgets->status_label), utf8);
        g_free(info);
    }

    free(pid1);
    free(pid2);
    free(pid3);

    if(utf8 != NULL)
        g_free(utf8);

    return (TRUE);
}
