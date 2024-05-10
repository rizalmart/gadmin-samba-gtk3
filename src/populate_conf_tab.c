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
#include "populate_conf_tab.h"



void populate_conf_tab(struct w *widgets)
{
    /* Show the configuration in the conf tab. */
    FILE *fp;
    long conf_size;
    char *old_buffy, *new_buffy;
    GtkTextBuffer *conf_textbuffer;
    gchar *utf8 = NULL;

    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        return;
    }

    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    old_buffy = allocate(conf_size + 1);
    new_buffy = allocate(conf_size + 1);

    if( conf_size > 1 )
        while(fgets(old_buffy, conf_size, fp) != NULL)
        {
            strcat(new_buffy, old_buffy);
        }
    fclose(fp);

    utf8 = g_locale_to_utf8(new_buffy, strlen(new_buffy), NULL, NULL, NULL);
    conf_textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->conf_textview));
    gtk_text_buffer_set_text(conf_textbuffer, utf8, strlen(utf8));

    free(old_buffy);
    free(new_buffy);

    if(utf8 != NULL)
        g_free(utf8);
}
