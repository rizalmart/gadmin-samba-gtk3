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
#include "add_macct.h"
#include "populate_maccts.h"
#include "select_first_macct.h"
#include "populate_macct_settings.h"
#include "commands.h"
#include "system_defines.h"

extern int activated;



/* Add the system and samba machine account */
void add_macct(struct w *widgets)
{
    FILE *fp;
    gchar *info, *path, *cmd, *machine_acct, *groupname;

    const gchar *username; // Machine name, same thing except ($)
    const gchar *comment;

    /* Get the users widget settings */
    username = gtk_entry_get_text(GTK_ENTRY(widgets->macct_set_entry[0]));
    comment = gtk_entry_get_text(GTK_ENTRY(widgets->macct_set_entry[1]));

    get_gadmin_samba_users();

    path = g_strdup_printf("%s", GADMIN_SAMBA_USERS);
    if( ! file_exists(path) )
        make_file_chmod(path, "0600");

    if( ! file_exists(path) )
    {
        info = g_strdup_printf(_("Failed creating missing file:\n%s\n"), path);
        show_info(info);
        g_free(info);
        g_free(path);
        return;
    }
    g_free(path);


    machine_acct = g_strdup_printf("%s$", username);
    groupname = g_strdup_printf("%s", "sambamachines");


    /* If the machine account exists, tell the user and return. */
    if( user_exists(username) )
    {
        info = g_strdup_printf(_("This machine account already exists.\n"));
        show_info(info);
        g_free(info);
        g_free(machine_acct);
        g_free(groupname);
        return;
    }


    /* Add the system group sambamachines if it doesnt exist */
    if( ! group_exists(groupname) )
    {
        cmd = g_strdup_printf("%s %s", "groupadd", groupname);
        if( ! run_command(cmd) )
        {
            printf("Error adding system group: %s\n", groupname);
            printf("The command was: %s\n", cmd);
            g_free(cmd);
            g_free(machine_acct);
            g_free(groupname);
            return;
        }
        g_free(cmd);
    }

    /* Add the system machine account to the sambamachines group */
    cmd = g_strdup_printf("%s '%s' -d '%s' -c '%s' -s '%s' -g '%s'", "useradd",
                          machine_acct, FALSESHELL, comment, FALSESHELL, groupname);
    if( ! run_command(cmd) )
    {
        printf("Error adding the system machine account: %s\n", machine_acct);
        printf("The command was: %s\n", cmd);
        g_free(cmd);
        g_free(machine_acct);
        g_free(groupname);
        return;
    }
    g_free(cmd);


    /* Make sure that the system machine account exists */
    if( ! user_exists(machine_acct) )
    {
        info = g_strdup_printf(_("Error adding system machine account: %s\n"), machine_acct);
        show_info(info);
        g_free(info);
        g_free(machine_acct);
        g_free(groupname);
        return;
    }


    /* Add the samba machine account. (No '$' or password is used) */
    cmd = g_strdup_printf("%s -a -m '%s'", PDBEDIT_BINARY, username);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed adding samba machine account: %s\n"), username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        g_free(machine_acct);
        g_free(groupname);
        return;
    }
    pclose(fp);
    g_free(cmd);


    g_free(machine_acct);
    g_free(groupname);

    /* Update the machine account tab */
    populate_maccts(widgets);
    select_first_macct(widgets);
    populate_macct_settings(widgets);
}
