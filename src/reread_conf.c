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
#include <unistd.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "commands.h"

extern int activated;
extern int global_winbindd_activated;
extern int global_nmbd_activated;


void reread_conf(struct w *widgets)
{
    /* Reread with a HUP and show any errors. */
    gchar *command, *test;

    if( ! activated )
        return;

    command = g_strdup_printf("killall -1 %s", SMBD_BINARY);
    if( ! run_command(command) )
    {
        printf("Restarting samba (smbd) failed\n");
        test = g_strdup_printf("killall -1 %s 2>&1", SMBD_BINARY);
        run_command_show_err(test);
        g_free(test);
    }
    g_free(command);

    if( global_nmbd_activated )
    {
        command = g_strdup_printf("killall -1 %s", NMBD_BINARY);
        if( ! run_command(command) )
        {
            printf("Restarting samba (nmbd) failed\n");
            test = g_strdup_printf("killall -1 %s 2>&1", NMBD_BINARY);
            run_command_show_err(test);
            g_free(test);
        }
        g_free(command);
    }

    if( global_winbindd_activated )
    {
        command = g_strdup_printf("killall -1 %s", WINBINDD_BINARY);
        if( ! run_command(command) )
        {
            printf("Restarting samba (winbindd) failed\n");
            test = g_strdup_printf("killall -1 %s 2>&1", WINBINDD_BINARY);
            run_command_show_err(test);
            g_free(test);
        }
        g_free(command);
    }
}
