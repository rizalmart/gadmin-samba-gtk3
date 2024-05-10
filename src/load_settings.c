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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "commands.h"
#include "allocate.h"
#include "load_settings.h"

extern int global_start_winbindd;
//extern char RESOLV_PATH_BUF[1024];


void load_settings(struct w *widgets)
{
    FILE *fp;
    long conf_size;
    char *line;
    gchar *settings_file;

    /* APPCONFDIR is defined at compiletime */
    settings_file = g_strdup_printf("%s/settings.conf", APPCONFDIR);
    if((fp = fopen(settings_file, "r")) == NULL)
    {
        /* No need to notify the user, itll be created on save */
        g_free(settings_file);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);
    line = allocate(conf_size + 1);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            if( strlen(line) > 1000 )
                continue;

            if( strstr(line, "start_winbindd:") )
            {
                if( strstr(line, "yes") )
                    global_start_winbindd = 1;
                else
                    global_start_winbindd = 0;
            }
        }
    fclose(fp);
    free(line);
    g_free(settings_file);
}
