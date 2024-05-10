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
#include <string.h>
#include <stdlib.h>
#include "widgets.h"
#include "gettext.h"
#include "allocate.h"
#include "show_info.h"

extern int MAX_READ_POPEN;



void set_version(struct w *widgets)
{
    FILE *fp;
    char *version;
    int ver_found = 0;
    gchar *command;
    gchar *ver = NULL;
    gchar *utf8 = NULL;
    gchar *info;

    /* smbd and nmbd should always be the same version so only check one. */
    command = g_strdup_printf("%s -V 2>&1", SMBD_BINARY);
    if((fp = popen(command, "r")) == NULL)
    {
        info = g_strdup_printf(_("Set version failed\n"));
        show_info(info);
        g_free(info);
        g_free(command);
        return;
    }
    g_free(command);

    version = allocate(MAX_READ_POPEN);

    while(fgets(version, MAX_READ_POPEN, fp) != NULL)
    {
        if( strstr(version, "Version") )
        {
            /* Insert trimmed version */
            ver = g_strdup_printf(_(" Information: SAMBA %s"), &version[8]);
            if( ver != NULL && strlen(ver) > 50 )
                ver[50] = '\0';
            utf8 = g_locale_to_utf8(ver, strlen(ver) - 1, NULL, NULL, NULL);
            gtk_label_set_text(GTK_LABEL(widgets->version_label), utf8);
            g_free(utf8);
            ver_found = 1;
        }
    }

    if( ! ver_found )
    {
        ver = g_strdup_printf(_(" samba (smbd) is not installed or not in your path."));
        utf8 = g_locale_to_utf8(ver, strlen(ver), NULL, NULL, NULL);
        gtk_label_set_text(GTK_LABEL(widgets->version_label), utf8);
        g_free(utf8);
        /* Make the labels background red and show the text */
//  Bold text: gchar *markup_text = g_strdup_printf("<span background=\"red\"><b>%s</b></span>", ver);
        gchar *markup_text = g_strdup_printf("<span background=\"red\">%s</span>", ver);
        gchar *plain_text;
        PangoAttrList *attr_list;
        pango_parse_markup(markup_text, -1, 0, &attr_list, &plain_text, NULL, NULL);
        gtk_label_set_attributes(GTK_LABEL(widgets->version_label), attr_list);
    }
    free(version);
    g_free(ver);
    pclose(fp);
}
