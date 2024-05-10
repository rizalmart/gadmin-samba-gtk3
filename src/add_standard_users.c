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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "gettext.h"
#include "widgets.h"
#include "allocate.h"
#include "commands.h"
#include "add_standard_users.h"
#include "system_defines.h"
#include "functions.h"
#include "commented.h"
#include "show_info.h"



/* Check if we have a good smbusers username map file */
int smb_usermap_ok(gchar * path)
{
    FILE *fp;
    char *line;
    long file_size;
    int valid_root = 0, valid_guest = 0;
    int retval = 0;

    if((fp = fopen(path, "r")) == NULL)
    {
        /* Dont show a dialog here */
        return retval;
    }
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    line = allocate(file_size + 1);

    if(file_size > 1)
    while(fgets(line, file_size, fp) != NULL)
    {
        if(strstr(line, "root") && strstr(line, "=")
           && strstr(line, "administrator") && !commented(line))
           valid_root = 1;

        if(strstr(line, "nobody") && strstr(line, "=")
           && strstr(line, "smbguest") && !commented(line))
           valid_guest = 1;
    }
    fclose(fp);
    free(line);

    if(valid_root && valid_guest)
        retval = 1;

    return retval;
}


int add_standard_users()
{
    /* The sambamachines group and users NOBODY and smbguest accounts 
       must exist so add them if they are missing */
    FILE *fp;
    gchar *cmd, *info, *path, *conf, *name;

    /* SYSCONFDIR/samba is required */
    cmd = g_strdup_printf("mkdir -p '%s/samba'", SYSCONFDIR);
    if( ! run_command(cmd) )
    {
        printf("Error creating directory: %s/samba\n", SYSCONFDIR);
        g_free(cmd);
        return 0;
    }
    g_free(cmd);

    /* SYSCONFDIR/samba/smbusers is required */
    cmd = g_strdup_printf("touch %s/samba/smbusers", SYSCONFDIR);
    if( ! run_command(cmd) )
    {
        printf("Error creating directory: %s/samba/smbusers\n", SYSCONFDIR);
        g_free(cmd);
        return 0;
    }
    g_free(cmd);

    /* SYSCONFDIR/samba/smb.conf is required */
    cmd = g_strdup_printf("touch %s/samba/smb.conf", SYSCONFDIR);
    if( ! run_command(cmd) )
    {
        printf("Error creating directory: %s/samba/smb.conf\n", SYSCONFDIR);
        g_free(cmd);
        return 0;
    }
    g_free(cmd);

    /* We want username mappings in /etc/samba/smbusers */
    conf = g_strconcat("# Unix_name = SMB_Name1 SMB_Name2 ...\n",
        "root = administrator\n", "nobody = guest smbguest pcguest\n", NULL);

    path = g_strdup_printf("%s", SAMBA_USERMAP);
    if( ! file_exists(path) || ! smb_usermap_ok(path) )
    {
        if((fp = fopen(path, "w+")) == NULL)
        {
            info = g_strdup_printf(_("Failed adding default smbusers file here:\n%s\n"),
                                   SAMBA_USERMAP);
            show_info(info);
            g_free(info);

            g_free(path);
            g_free(conf);
            return 0;
        }
        fputs(conf, fp);
        fclose(fp);

        info = g_strdup_printf(_
            ("A new smbusers map file was added here:\n%s\n It has the following contents: \"root = administrator\" and \"nobody = smbguest\".\n"),
                                                  SAMBA_USERMAP);
        show_info(info);
        g_free(info);
    }
    g_free(path);
    g_free(conf);

    /* Add the NOBODY system group if it doesnt exist */
    name = g_strdup_printf("%s", NOBODY);
    if( ! group_exists(name) )
    {
        cmd = g_strdup_printf("%s %s", "groupadd", name);
        if( ! run_command(cmd) )
        {
            printf("Error adding system group: %s\n", name);
            printf("The command was: %s\n", cmd);
            g_free(cmd);
            g_free(name);
            return 0;
        }
        g_free(cmd);
    }
    g_free(name);

    /* Add the NOBODY system user to the NOBODY group as a locked account */
    name = g_strdup_printf("%s", NOBODY);
    if( ! user_exists(name) )
    {
        cmd = g_strdup_printf("%s '%s' -d /dev/null -c Nobody -s /dev/null -g '%s'", "useradd", name,
                              name);
        if( ! run_command(cmd) )
        {
            printf("Error adding the system user: %s\n", name);
            printf("The command was: %s\n", cmd);
            g_free(cmd);
            g_free(name);
            return 0;
        }
        g_free(cmd);

        /* To be really sure we check it again with the same function */
        if( ! user_exists(name) )
        {
            info = g_strdup_printf(_("Failed adding the system user:\n%s\n"), name);
            show_info(info);
            g_free(info);
            g_free(name);
            return 0;
        }
    }
    g_free(name);


    /* Add the smbguest system user to the NOBODY group and lock the account */
    name = g_strdup_printf("%s", "smbguest");
    if( ! user_exists(name) )
    {
        /* Add the system user to the NOBODY group */
        cmd = g_strdup_printf("%s '%s' -d /dev/null -c 'Samba guest account' -s /dev/null -g '%s'",
                              "useradd", name, NOBODY);
        if( ! run_command(cmd) )
        {
            printf("Error adding the system user: %s\n", name);
            printf("The command was: %s\n", cmd);
            g_free(cmd);
            g_free(name);
            return 0;
        }
        g_free(cmd);

        /* To be really sure we check it again with the same function */
        if( ! user_exists(name) )
        {
            info = g_strdup_printf(_("Failed adding the system user:\n%s\n"), name);
            show_info(info);
            g_free(info);
            g_free(name);
            return 0;
        }
    }
    g_free(name);


    /* The smbguest user also needs to be added to the samba user/pass file as a locked user */
    name = g_strdup_printf("%s", "smbguest");
    if( ! smbuser_exists(name) )
    {
        if( ! add_locked_samba_user(name) )
        {
            printf("Error adding standard samba user: %s\n", name);
        }
    }
    g_free(name);


    /* The sambamachines account also needs to exist in order to automatically 
       add valid(username+password) connecting computers BIOS-id's as computer accounts */
    name = g_strdup_printf("%s", "sambamachines");
    if( ! group_exists(name) )
    {
        cmd = g_strdup_printf("%s %s", "groupadd", name);
        if( ! run_command(cmd) )
        {
            printf("Error adding system group: %s\n", name);
            printf("The command was: %s\n", cmd);
            g_free(cmd);
            g_free(name);
            return 0;
        }
        g_free(cmd);
    }
    g_free(name);


    /* The samba root user also needs to be added to be able to join this domain */
    name = g_strdup_printf("%s", "root");
    if( ! smbuser_exists(name) )
    {
        if( ! add_locked_samba_user(name) )
        {
            printf("Error adding standard samba user: %s\n", name);
        }
    }
    g_free(name);

    return 1;
}
