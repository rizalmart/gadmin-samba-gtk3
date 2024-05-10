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
#include <gtk/gtk.h>
#include "gettext.h"
#include "allocate.h"
#include "widgets.h"
#include "functions.h"
#include "populate_macct_settings.h"
#include "chars_are_digits.h"
#include "show_info.h"


extern char global_macct_name[1024];



void populate_macct_settings(struct w *widgets)
{
    gchar *info;
    gchar *utf8 = NULL;
    int i = 0;
    gchar *what, *val, *machine_name;

    machine_name = g_strdup_printf("%s", global_macct_name);

    info = g_strdup_printf("%s", global_macct_name);
    if( info == NULL || strlen(info) < 1 )
    {
        /* User not specified, clear all widgets */
        for(i = 0; i < 2; i++)
            gtk_entry_set_text(GTK_ENTRY(widgets->macct_set_entry[i]), "");
    }
    g_free(info);

    /* Set the comment before modding the machine name */
    what = g_strdup_printf("comment");
    val = get_user_setting(machine_name, what);
    gtk_entry_set_text(GTK_ENTRY(widgets->macct_set_entry[1]), val);
    g_free(what);
    g_free(val);

    /* Set the machine name */
    if( machine_name != NULL && strlen(machine_name) > 1 )
        machine_name[strlen(machine_name) - 1] = '\0';

    gtk_entry_set_text(GTK_ENTRY(widgets->macct_set_entry[0]), machine_name);

    if(utf8 != NULL)
        g_free(utf8);
}
