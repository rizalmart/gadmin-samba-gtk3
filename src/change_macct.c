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
#include "functions.h"
#include "show_info.h"
#include "allocate.h"
#include "change_macct.h"
#include "populate_maccts.h"
#include "select_first_macct.h"
#include "populate_macct_settings.h"
#include "commands.h"



/* Add the system and samba machine account */
void change_macct(struct w *widgets)
{
    gchar *info, *cmd, *machine_acct;

    const gchar *username; // Machine name, same thing except ($)
    const gchar *comment;

    /* Get the users widget settings */
    username = gtk_entry_get_text(GTK_ENTRY(widgets->macct_set_entry[0]));
    comment = gtk_entry_get_text(GTK_ENTRY(widgets->macct_set_entry[1]));
    machine_acct = g_strdup_printf("%s$", username);

    /* If the machine account doesnt exist, tell the user and return. */
    if( ! user_exists(machine_acct) )
    {
        info = g_strdup_printf(_("The machine account: %s\ncan not be changed because it does not exist.\n"),
                                 machine_acct);
        show_info(info);
        g_free(info);
        g_free(machine_acct);
        return;
    }

    /* Change the machine account comment */
    cmd = g_strdup_printf("%s -c '%s' '%s'\n", "usermod", comment, machine_acct);
    if( ! run_command(cmd) )
    {
        printf("Error changing the machine accounts system comment: %s\n", machine_acct);
        printf("The command was: %s\n", cmd);
        g_free(cmd);
        g_free(machine_acct);
        return;
    }
    g_free(cmd);

    g_free(machine_acct);

    /* Update the machine account tab */
    populate_maccts(widgets);
    select_first_macct(widgets);
    populate_macct_settings(widgets);
}
