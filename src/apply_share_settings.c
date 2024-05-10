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
#include <stdlib.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "allocate.h"
#include "commented.h"
#include "reread_conf.h"
#include "show_info.h"
#include "apply_share_settings.h"
#include "add_share.h"
#include "populate_share_settings.h"
#include "populate_server_settings.h"
#include "populate_conf_tab.h"

extern char global_share_name[1024];
extern int activated;


/* Changes the selected share configuration by removing
   the old old share and adding a new one via add_share.c */
void apply_share_settings(struct w *widgets)
{
    FILE *fp;
    char *line, *new_conf;
    long conf_size;
    int found_share = 0;
    gchar *info;

    gchar *share_name = g_strdup_printf("[%s]", global_share_name);

    /* FIXME: Some default shares cant be changed without loosing options atm so dont */
    if( ! strcmp(share_name, "[homes]" )
        || ! strcmp(share_name, "[profiles]")
        || ! strcmp(share_name, "[netlogon]")
        || ! strcmp(share_name, "[printers]")
        || ! strcmp(share_name, "[pdf-documents]") || ! strcmp(share_name, "[pdf-printer]") )
    {
        g_free(share_name);
        info = g_strdup_printf(_("These default shares cant be changed at the moment.\nEdit the configuration for now\n"));
        show_info(info);
        g_free(info);
        return;
    }

    /* First delete the old share definition */
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
    new_conf = allocate(conf_size + 1);

    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        /* Skip commented lines */
        if( commented(line) )
            continue;

        /* The selected share is found, skip it */
        if( strstr(line, share_name) )
        {
            found_share = 1;
            while(fgets(line, conf_size, fp) != NULL)
            {
                /* Skip commented lines */
                if( commented(line) )
                    continue;

                /* If a new share is found, break */
                if( strstr(line, "[") && strstr(line, "]") )
                {
                    strcat(new_conf, line);
                    break;
                }
            }
        }
        else
            strcat(new_conf, line);
    }
    fclose(fp);
    free(line);
    g_free(share_name);

    if( ! found_share )
    {
        free(new_conf);
//      info...
        return;
    }

    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        /* Dont show a popup */
        printf("Error writing: [%s] when deleting a share\n", SAMBA_CONF);
        return;
    }
    fputs(new_conf, fp);
    fclose(fp);
    free(new_conf);

    /* Add the new share definition */
    add_share(widgets);

    /* Reread the conf */
    reread_conf(widgets);
}
