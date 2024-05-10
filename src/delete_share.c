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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "gettext.h"
#include "widgets.h"
#include "allocate.h"
#include "chars_are_digits.h"
#include "delete_share.h"
#include "reread_conf.h"
#include "commented.h"
#include "get_option_pos.h"
#include "show_info.h"
#include "functions.h"
#include "populate_shares.h"
#include "populate_share_settings.h"
#include "commands.h"

extern char global_share_name[1024];



void delete_share(struct w *widgets)
{
    /* Deletes the selected share */
    FILE *fp;
    char *line, *new_conf;
    long conf_size;
    int found_share = 0;

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

    gchar *share_name = g_strdup_printf("[%s]", global_share_name);

    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        /* Skip commented lines */
        if( commented(line) )
            continue;

        /* The selected share is found */
        if( strstr(line, share_name) )
        {
            found_share = 1;
            while(fgets(line, conf_size, fp) != NULL)
            {
                /* Skip commented lines */
                if( commented(line) )
                    continue;

                /* If a new share is found, break */
                if( strstr(line, "[") && strstr(line, "]") && ! commented(line) )
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

//    if( ! found_share )
//    {
//  Tell the user.... strcat(global_share_name, "");
//  free(new_conf);
//      return;
//    }

    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        /* Dont show a popup */
        printf("Error writing: [%s] for deleting shares\n", SAMBA_CONF);
        return;
    }
    fputs(new_conf, fp);
    fclose(fp);
    free(new_conf);

    fix_newlines_in_conf();

    populate_shares(widgets);
    populate_share_settings(widgets);

    reread_conf(widgets);
}
